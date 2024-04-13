static int proc_do_submiturb(struct usb_dev_state *ps, struct usbdevfs_urb *uurb,
			struct usbdevfs_iso_packet_desc __user *iso_frame_desc,
			void __user *arg)
{
	struct usbdevfs_iso_packet_desc *isopkt = NULL;
	struct usb_host_endpoint *ep;
	struct async *as = NULL;
	struct usb_ctrlrequest *dr = NULL;
	unsigned int u, totlen, isofrmlen;
	int i, ret, is_in, num_sgs = 0, ifnum = -1;
	int number_of_packets = 0;
	unsigned int stream_id = 0;
	void *buf;

	if (uurb->flags & ~(USBDEVFS_URB_ISO_ASAP |
				USBDEVFS_URB_SHORT_NOT_OK |
				USBDEVFS_URB_BULK_CONTINUATION |
				USBDEVFS_URB_NO_FSBR |
				USBDEVFS_URB_ZERO_PACKET |
				USBDEVFS_URB_NO_INTERRUPT))
		return -EINVAL;
	if (uurb->buffer_length > 0 && !uurb->buffer)
		return -EINVAL;
	if (!(uurb->type == USBDEVFS_URB_TYPE_CONTROL &&
	    (uurb->endpoint & ~USB_ENDPOINT_DIR_MASK) == 0)) {
		ifnum = findintfep(ps->dev, uurb->endpoint);
		if (ifnum < 0)
			return ifnum;
		ret = checkintf(ps, ifnum);
		if (ret)
			return ret;
	}
	ep = ep_to_host_endpoint(ps->dev, uurb->endpoint);
	if (!ep)
		return -ENOENT;
	is_in = (uurb->endpoint & USB_ENDPOINT_DIR_MASK) != 0;

	u = 0;
	switch (uurb->type) {
	case USBDEVFS_URB_TYPE_CONTROL:
		if (!usb_endpoint_xfer_control(&ep->desc))
			return -EINVAL;
		/* min 8 byte setup packet */
		if (uurb->buffer_length < 8)
			return -EINVAL;
		dr = kmalloc(sizeof(struct usb_ctrlrequest), GFP_KERNEL);
		if (!dr)
			return -ENOMEM;
		if (copy_from_user(dr, uurb->buffer, 8)) {
			ret = -EFAULT;
			goto error;
		}
		if (uurb->buffer_length < (le16_to_cpup(&dr->wLength) + 8)) {
			ret = -EINVAL;
			goto error;
		}
		ret = check_ctrlrecip(ps, dr->bRequestType, dr->bRequest,
				      le16_to_cpup(&dr->wIndex));
		if (ret)
			goto error;
		uurb->buffer_length = le16_to_cpup(&dr->wLength);
		uurb->buffer += 8;
		if ((dr->bRequestType & USB_DIR_IN) && uurb->buffer_length) {
			is_in = 1;
			uurb->endpoint |= USB_DIR_IN;
		} else {
			is_in = 0;
			uurb->endpoint &= ~USB_DIR_IN;
		}
		snoop(&ps->dev->dev, "control urb: bRequestType=%02x "
			"bRequest=%02x wValue=%04x "
			"wIndex=%04x wLength=%04x\n",
			dr->bRequestType, dr->bRequest,
			__le16_to_cpup(&dr->wValue),
			__le16_to_cpup(&dr->wIndex),
			__le16_to_cpup(&dr->wLength));
		u = sizeof(struct usb_ctrlrequest);
		break;

	case USBDEVFS_URB_TYPE_BULK:
		switch (usb_endpoint_type(&ep->desc)) {
		case USB_ENDPOINT_XFER_CONTROL:
		case USB_ENDPOINT_XFER_ISOC:
			return -EINVAL;
		case USB_ENDPOINT_XFER_INT:
			/* allow single-shot interrupt transfers */
			uurb->type = USBDEVFS_URB_TYPE_INTERRUPT;
			goto interrupt_urb;
		}
		num_sgs = DIV_ROUND_UP(uurb->buffer_length, USB_SG_SIZE);
		if (num_sgs == 1 || num_sgs > ps->dev->bus->sg_tablesize)
			num_sgs = 0;
		if (ep->streams)
			stream_id = uurb->stream_id;
		break;

	case USBDEVFS_URB_TYPE_INTERRUPT:
		if (!usb_endpoint_xfer_int(&ep->desc))
			return -EINVAL;
 interrupt_urb:
		break;

	case USBDEVFS_URB_TYPE_ISO:
		/* arbitrary limit */
		if (uurb->number_of_packets < 1 ||
		    uurb->number_of_packets > 128)
			return -EINVAL;
		if (!usb_endpoint_xfer_isoc(&ep->desc))
			return -EINVAL;
		number_of_packets = uurb->number_of_packets;
		isofrmlen = sizeof(struct usbdevfs_iso_packet_desc) *
				   number_of_packets;
		isopkt = memdup_user(iso_frame_desc, isofrmlen);
		if (IS_ERR(isopkt)) {
			ret = PTR_ERR(isopkt);
			isopkt = NULL;
			goto error;
		}
		for (totlen = u = 0; u < number_of_packets; u++) {
			/*
			 * arbitrary limit need for USB 3.0
			 * bMaxBurst (0~15 allowed, 1~16 packets)
			 * bmAttributes (bit 1:0, mult 0~2, 1~3 packets)
			 * sizemax: 1024 * 16 * 3 = 49152
			 */
			if (isopkt[u].length > 49152) {
				ret = -EINVAL;
				goto error;
			}
			totlen += isopkt[u].length;
		}
		u *= sizeof(struct usb_iso_packet_descriptor);
		uurb->buffer_length = totlen;
		break;

	default:
		return -EINVAL;
	}

	if (uurb->buffer_length >= USBFS_XFER_MAX) {
		ret = -EINVAL;
		goto error;
	}
	if (uurb->buffer_length > 0 &&
			!access_ok(is_in ? VERIFY_WRITE : VERIFY_READ,
				uurb->buffer, uurb->buffer_length)) {
		ret = -EFAULT;
		goto error;
	}
	as = alloc_async(number_of_packets);
	if (!as) {
		ret = -ENOMEM;
		goto error;
	}

	as->usbm = find_memory_area(ps, uurb);
	if (IS_ERR(as->usbm)) {
		ret = PTR_ERR(as->usbm);
		as->usbm = NULL;
		goto error;
	}

	/* do not use SG buffers when memory mapped segments
	 * are in use
	 */
	if (as->usbm)
		num_sgs = 0;

	u += sizeof(struct async) + sizeof(struct urb) + uurb->buffer_length +
	     num_sgs * sizeof(struct scatterlist);
	ret = usbfs_increase_memory_usage(u);
	if (ret)
		goto error;
	as->mem_usage = u;

	if (num_sgs) {
		as->urb->sg = kmalloc(num_sgs * sizeof(struct scatterlist),
				      GFP_KERNEL);
		if (!as->urb->sg) {
			ret = -ENOMEM;
			goto error;
		}
		as->urb->num_sgs = num_sgs;
		sg_init_table(as->urb->sg, as->urb->num_sgs);

		totlen = uurb->buffer_length;
		for (i = 0; i < as->urb->num_sgs; i++) {
			u = (totlen > USB_SG_SIZE) ? USB_SG_SIZE : totlen;
			buf = kmalloc(u, GFP_KERNEL);
			if (!buf) {
				ret = -ENOMEM;
				goto error;
			}
			sg_set_buf(&as->urb->sg[i], buf, u);

			if (!is_in) {
				if (copy_from_user(buf, uurb->buffer, u)) {
					ret = -EFAULT;
					goto error;
				}
				uurb->buffer += u;
			}
			totlen -= u;
		}
	} else if (uurb->buffer_length > 0) {
		if (as->usbm) {
			unsigned long uurb_start = (unsigned long)uurb->buffer;

			as->urb->transfer_buffer = as->usbm->mem +
					(uurb_start - as->usbm->vm_start);
		} else {
			as->urb->transfer_buffer = kmalloc(uurb->buffer_length,
					GFP_KERNEL);
			if (!as->urb->transfer_buffer) {
				ret = -ENOMEM;
				goto error;
			}
			if (!is_in) {
				if (copy_from_user(as->urb->transfer_buffer,
						   uurb->buffer,
						   uurb->buffer_length)) {
					ret = -EFAULT;
					goto error;
				}
			} else if (uurb->type == USBDEVFS_URB_TYPE_ISO) {
				/*
				 * Isochronous input data may end up being
				 * discontiguous if some of the packets are
				 * short. Clear the buffer so that the gaps
				 * don't leak kernel data to userspace.
				 */
				memset(as->urb->transfer_buffer, 0,
						uurb->buffer_length);
			}
		}
	}
	as->urb->dev = ps->dev;
	as->urb->pipe = (uurb->type << 30) |
			__create_pipe(ps->dev, uurb->endpoint & 0xf) |
			(uurb->endpoint & USB_DIR_IN);

	/* This tedious sequence is necessary because the URB_* flags
	 * are internal to the kernel and subject to change, whereas
	 * the USBDEVFS_URB_* flags are a user API and must not be changed.
	 */
	u = (is_in ? URB_DIR_IN : URB_DIR_OUT);
	if (uurb->flags & USBDEVFS_URB_ISO_ASAP)
		u |= URB_ISO_ASAP;
	if (uurb->flags & USBDEVFS_URB_SHORT_NOT_OK && is_in)
		u |= URB_SHORT_NOT_OK;
	if (uurb->flags & USBDEVFS_URB_NO_FSBR)
		u |= URB_NO_FSBR;
	if (uurb->flags & USBDEVFS_URB_ZERO_PACKET)
		u |= URB_ZERO_PACKET;
	if (uurb->flags & USBDEVFS_URB_NO_INTERRUPT)
		u |= URB_NO_INTERRUPT;
	as->urb->transfer_flags = u;

	as->urb->transfer_buffer_length = uurb->buffer_length;
	as->urb->setup_packet = (unsigned char *)dr;
	dr = NULL;
	as->urb->start_frame = uurb->start_frame;
	as->urb->number_of_packets = number_of_packets;
	as->urb->stream_id = stream_id;
	if (uurb->type == USBDEVFS_URB_TYPE_ISO ||
			ps->dev->speed == USB_SPEED_HIGH)
		as->urb->interval = 1 << min(15, ep->desc.bInterval - 1);
	else
		as->urb->interval = ep->desc.bInterval;
	as->urb->context = as;
	as->urb->complete = async_completed;
	for (totlen = u = 0; u < number_of_packets; u++) {
		as->urb->iso_frame_desc[u].offset = totlen;
		as->urb->iso_frame_desc[u].length = isopkt[u].length;
		totlen += isopkt[u].length;
	}
	kfree(isopkt);
	isopkt = NULL;
	as->ps = ps;
	as->userurb = arg;
	if (as->usbm) {
		unsigned long uurb_start = (unsigned long)uurb->buffer;

		as->urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;
		as->urb->transfer_dma = as->usbm->dma_handle +
				(uurb_start - as->usbm->vm_start);
	} else if (is_in && uurb->buffer_length > 0)
		as->userbuffer = uurb->buffer;
	as->signr = uurb->signr;
	as->ifnum = ifnum;
	as->pid = get_pid(task_pid(current));
	as->cred = get_current_cred();
	security_task_getsecid(current, &as->secid);
	snoop_urb(ps->dev, as->userurb, as->urb->pipe,
			as->urb->transfer_buffer_length, 0, SUBMIT,
			NULL, 0);
	if (!is_in)
		snoop_urb_data(as->urb, as->urb->transfer_buffer_length);

	async_newpending(as);

	if (usb_endpoint_xfer_bulk(&ep->desc)) {
		spin_lock_irq(&ps->lock);

		/* Not exactly the endpoint address; the direction bit is
		 * shifted to the 0x10 position so that the value will be
		 * between 0 and 31.
		 */
		as->bulk_addr = usb_endpoint_num(&ep->desc) |
			((ep->desc.bEndpointAddress & USB_ENDPOINT_DIR_MASK)
				>> 3);

		/* If this bulk URB is the start of a new transfer, re-enable
		 * the endpoint.  Otherwise mark it as a continuation URB.
		 */
		if (uurb->flags & USBDEVFS_URB_BULK_CONTINUATION)
			as->bulk_status = AS_CONTINUATION;
		else
			ps->disabled_bulk_eps &= ~(1 << as->bulk_addr);

		/* Don't accept continuation URBs if the endpoint is
		 * disabled because of an earlier error.
		 */
		if (ps->disabled_bulk_eps & (1 << as->bulk_addr))
			ret = -EREMOTEIO;
		else
			ret = usb_submit_urb(as->urb, GFP_ATOMIC);
		spin_unlock_irq(&ps->lock);
	} else {
		ret = usb_submit_urb(as->urb, GFP_KERNEL);
	}

	if (ret) {
		dev_printk(KERN_DEBUG, &ps->dev->dev,
			   "usbfs: usb_submit_urb returned %d\n", ret);
		snoop_urb(ps->dev, as->userurb, as->urb->pipe,
				0, ret, COMPLETE, NULL, 0);
		async_removepending(as);
		goto error;
	}
	return 0;

 error:
	if (as && as->usbm)
		dec_usb_memory_use_count(as->usbm, &as->usbm->urb_use_count);
	kfree(isopkt);
	kfree(dr);
	if (as)
		free_async(as);
	return ret;
}
