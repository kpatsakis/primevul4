static void oz_process_ep0_urb(struct oz_hcd *ozhcd, struct urb *urb,
		gfp_t mem_flags)
{
	struct usb_ctrlrequest *setup;
	unsigned windex;
	unsigned wvalue;
	unsigned wlength;
	void *hpd;
	u8 req_id;
	int rc = 0;
	unsigned complete = 0;

	int port_ix = -1;
	struct oz_port *port = NULL;

	oz_dbg(URB, "[%s]:(%p)\n", __func__, urb);
	port_ix = oz_get_port_from_addr(ozhcd, urb->dev->devnum);
	if (port_ix < 0) {
		rc = -EPIPE;
		goto out;
	}
	port =  &ozhcd->ports[port_ix];
	if (((port->flags & OZ_PORT_F_PRESENT) == 0)
		|| (port->flags & OZ_PORT_F_DYING)) {
		oz_dbg(ON, "Refusing URB port_ix = %d devnum = %d\n",
		       port_ix, urb->dev->devnum);
		rc = -EPIPE;
		goto out;
	}
	/* Store port in private context data.
	 */
	urb->hcpriv = port;
	setup = (struct usb_ctrlrequest *)urb->setup_packet;
	windex = le16_to_cpu(setup->wIndex);
	wvalue = le16_to_cpu(setup->wValue);
	wlength = le16_to_cpu(setup->wLength);
	oz_dbg(CTRL_DETAIL, "bRequestType = %x\n", setup->bRequestType);
	oz_dbg(CTRL_DETAIL, "bRequest = %x\n", setup->bRequest);
	oz_dbg(CTRL_DETAIL, "wValue = %x\n", wvalue);
	oz_dbg(CTRL_DETAIL, "wIndex = %x\n", windex);
	oz_dbg(CTRL_DETAIL, "wLength = %x\n", wlength);

	req_id = port->next_req_id++;
	hpd = oz_claim_hpd(port);
	if (hpd == NULL) {
		oz_dbg(ON, "Cannot claim port\n");
		rc = -EPIPE;
		goto out;
	}

	if ((setup->bRequestType & USB_TYPE_MASK) == USB_TYPE_STANDARD) {
		/* Standard requests
		 */
		switch (setup->bRequest) {
		case USB_REQ_GET_DESCRIPTOR:
			oz_dbg(ON, "USB_REQ_GET_DESCRIPTOR - req\n");
			break;
		case USB_REQ_SET_ADDRESS:
			oz_dbg(ON, "USB_REQ_SET_ADDRESS - req\n");
			oz_dbg(ON, "Port %d address is 0x%x\n",
			       ozhcd->conn_port,
			       (u8)le16_to_cpu(setup->wValue));
			spin_lock_bh(&ozhcd->hcd_lock);
			if (ozhcd->conn_port >= 0) {
				ozhcd->ports[ozhcd->conn_port].bus_addr =
					(u8)le16_to_cpu(setup->wValue);
				oz_dbg(ON, "Clearing conn_port\n");
				ozhcd->conn_port = -1;
			}
			spin_unlock_bh(&ozhcd->hcd_lock);
			complete = 1;
			break;
		case USB_REQ_SET_CONFIGURATION:
			oz_dbg(ON, "USB_REQ_SET_CONFIGURATION - req\n");
			break;
		case USB_REQ_GET_CONFIGURATION:
			/* We short circuit this case and reply directly since
			 * we have the selected configuration number cached.
			 */
			oz_dbg(ON, "USB_REQ_GET_CONFIGURATION - reply now\n");
			if (urb->transfer_buffer_length >= 1) {
				urb->actual_length = 1;
				*((u8 *)urb->transfer_buffer) =
					port->config_num;
				complete = 1;
			} else {
				rc = -EPIPE;
			}
			break;
		case USB_REQ_GET_INTERFACE:
			/* We short circuit this case and reply directly since
			 * we have the selected interface alternative cached.
			 */
			oz_dbg(ON, "USB_REQ_GET_INTERFACE - reply now\n");
			if (urb->transfer_buffer_length >= 1) {
				urb->actual_length = 1;
				*((u8 *)urb->transfer_buffer) =
					port->iface[(u8)windex].alt;
				oz_dbg(ON, "interface = %d alt = %d\n",
				       windex, port->iface[(u8)windex].alt);
				complete = 1;
			} else {
				rc = -EPIPE;
			}
			break;
		case USB_REQ_SET_INTERFACE:
			oz_dbg(ON, "USB_REQ_SET_INTERFACE - req\n");
			break;
		}
	}
	if (!rc && !complete) {
		int data_len = 0;

		if ((setup->bRequestType & USB_DIR_IN) == 0)
			data_len = wlength;
		urb->actual_length = data_len;
		if (oz_usb_control_req(port->hpd, req_id, setup,
				urb->transfer_buffer, data_len)) {
			rc = -ENOMEM;
		} else {
			/* Note: we are queuing the request after we have
			 * submitted it to be transmitted. If the request were
			 * to complete before we queued it then it would not
			 * be found in the queue. It seems impossible for
			 * this to happen but if it did the request would
			 * be resubmitted so the problem would hopefully
			 * resolve itself. Putting the request into the
			 * queue before it has been sent is worse since the
			 * urb could be cancelled while we are using it
			 * to build the request.
			 */
			if (oz_enqueue_ep_urb(port, 0, 0, urb, req_id))
				rc = -ENOMEM;
		}
	}
	oz_usb_put(hpd);
out:
	if (rc || complete) {
		oz_dbg(ON, "Completing request locally\n");
		oz_complete_urb(ozhcd->hcd, urb, rc);
	} else {
		oz_usb_request_heartbeat(port->hpd);
	}
}
