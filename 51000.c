static int ffs_func_setup(struct usb_function *f,
			  const struct usb_ctrlrequest *creq)
{
	struct ffs_function *func = ffs_func_from_usb(f);
	struct ffs_data *ffs = func->ffs;
	unsigned long flags;
	int ret;

	ENTER();

	pr_vdebug("creq->bRequestType = %02x\n", creq->bRequestType);
	pr_vdebug("creq->bRequest     = %02x\n", creq->bRequest);
	pr_vdebug("creq->wValue       = %04x\n", le16_to_cpu(creq->wValue));
	pr_vdebug("creq->wIndex       = %04x\n", le16_to_cpu(creq->wIndex));
	pr_vdebug("creq->wLength      = %04x\n", le16_to_cpu(creq->wLength));

	/*
	 * Most requests directed to interface go through here
	 * (notable exceptions are set/get interface) so we need to
	 * handle them.  All other either handled by composite or
	 * passed to usb_configuration->setup() (if one is set).  No
	 * matter, we will handle requests directed to endpoint here
	 * as well (as it's straightforward) but what to do with any
	 * other request?
	 */
	if (ffs->state != FFS_ACTIVE)
		return -ENODEV;

	switch (creq->bRequestType & USB_RECIP_MASK) {
	case USB_RECIP_INTERFACE:
		ret = ffs_func_revmap_intf(func, le16_to_cpu(creq->wIndex));
		if (unlikely(ret < 0))
			return ret;
		break;

	case USB_RECIP_ENDPOINT:
		ret = ffs_func_revmap_ep(func, le16_to_cpu(creq->wIndex));
		if (unlikely(ret < 0))
			return ret;
		if (func->ffs->user_flags & FUNCTIONFS_VIRTUAL_ADDR)
			ret = func->ffs->eps_addrmap[ret];
		break;

	default:
		return -EOPNOTSUPP;
	}

	spin_lock_irqsave(&ffs->ev.waitq.lock, flags);
	ffs->ev.setup = *creq;
	ffs->ev.setup.wIndex = cpu_to_le16(ret);
	__ffs_event_add(ffs, FUNCTIONFS_SETUP);
	spin_unlock_irqrestore(&ffs->ev.waitq.lock, flags);

	return 0;
}
