static int ffs_func_eps_enable(struct ffs_function *func)
{
	struct ffs_data *ffs      = func->ffs;
	struct ffs_ep *ep         = func->eps;
	struct ffs_epfile *epfile = ffs->epfiles;
	unsigned count            = ffs->eps_count;
	unsigned long flags;
	int ret = 0;

	spin_lock_irqsave(&func->ffs->eps_lock, flags);
	do {
		struct usb_endpoint_descriptor *ds;
		int desc_idx;

		if (ffs->gadget->speed == USB_SPEED_SUPER)
			desc_idx = 2;
		else if (ffs->gadget->speed == USB_SPEED_HIGH)
			desc_idx = 1;
		else
			desc_idx = 0;

		/* fall-back to lower speed if desc missing for current speed */
		do {
			ds = ep->descs[desc_idx];
		} while (!ds && --desc_idx >= 0);

		if (!ds) {
			ret = -EINVAL;
			break;
		}

		ep->ep->driver_data = ep;
		ep->ep->desc = ds;
		ret = usb_ep_enable(ep->ep);
		if (likely(!ret)) {
			epfile->ep = ep;
			epfile->in = usb_endpoint_dir_in(ds);
			epfile->isoc = usb_endpoint_xfer_isoc(ds);
		} else {
			break;
		}

		wake_up(&epfile->wait);

		++ep;
		++epfile;
	} while (--count);
	spin_unlock_irqrestore(&func->ffs->eps_lock, flags);

	return ret;
}
