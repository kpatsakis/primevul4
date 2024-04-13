static int __ffs_func_bind_do_descs(enum ffs_entity_type type, u8 *valuep,
				    struct usb_descriptor_header *desc,
				    void *priv)
{
	struct usb_endpoint_descriptor *ds = (void *)desc;
	struct ffs_function *func = priv;
	struct ffs_ep *ffs_ep;
	unsigned ep_desc_id;
	int idx;
	static const char *speed_names[] = { "full", "high", "super" };

	if (type != FFS_DESCRIPTOR)
		return 0;

	/*
	 * If ss_descriptors is not NULL, we are reading super speed
	 * descriptors; if hs_descriptors is not NULL, we are reading high
	 * speed descriptors; otherwise, we are reading full speed
	 * descriptors.
	 */
	if (func->function.ss_descriptors) {
		ep_desc_id = 2;
		func->function.ss_descriptors[(long)valuep] = desc;
	} else if (func->function.hs_descriptors) {
		ep_desc_id = 1;
		func->function.hs_descriptors[(long)valuep] = desc;
	} else {
		ep_desc_id = 0;
		func->function.fs_descriptors[(long)valuep]    = desc;
	}

	if (!desc || desc->bDescriptorType != USB_DT_ENDPOINT)
		return 0;

	idx = ffs_ep_addr2idx(func->ffs, ds->bEndpointAddress) - 1;
	if (idx < 0)
		return idx;

	ffs_ep = func->eps + idx;

	if (unlikely(ffs_ep->descs[ep_desc_id])) {
		pr_err("two %sspeed descriptors for EP %d\n",
			  speed_names[ep_desc_id],
			  ds->bEndpointAddress & USB_ENDPOINT_NUMBER_MASK);
		return -EINVAL;
	}
	ffs_ep->descs[ep_desc_id] = ds;

	ffs_dump_mem(": Original  ep desc", ds, ds->bLength);
	if (ffs_ep->ep) {
		ds->bEndpointAddress = ffs_ep->descs[0]->bEndpointAddress;
		if (!ds->wMaxPacketSize)
			ds->wMaxPacketSize = ffs_ep->descs[0]->wMaxPacketSize;
	} else {
		struct usb_request *req;
		struct usb_ep *ep;
		u8 bEndpointAddress;

		/*
		 * We back up bEndpointAddress because autoconfig overwrites
		 * it with physical endpoint address.
		 */
		bEndpointAddress = ds->bEndpointAddress;
		pr_vdebug("autoconfig\n");
		ep = usb_ep_autoconfig(func->gadget, ds);
		if (unlikely(!ep))
			return -ENOTSUPP;
		ep->driver_data = func->eps + idx;

		req = usb_ep_alloc_request(ep, GFP_KERNEL);
		if (unlikely(!req))
			return -ENOMEM;

		ffs_ep->ep  = ep;
		ffs_ep->req = req;
		func->eps_revmap[ds->bEndpointAddress &
				 USB_ENDPOINT_NUMBER_MASK] = idx + 1;
		/*
		 * If we use virtual address mapping, we restore
		 * original bEndpointAddress value.
		 */
		if (func->ffs->user_flags & FUNCTIONFS_VIRTUAL_ADDR)
			ds->bEndpointAddress = bEndpointAddress;
	}
	ffs_dump_mem(": Rewritten ep desc", ds, ds->bLength);

	return 0;
}
