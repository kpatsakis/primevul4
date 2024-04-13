static long ffs_epfile_ioctl(struct file *file, unsigned code,
			     unsigned long value)
{
	struct ffs_epfile *epfile = file->private_data;
	int ret;

	ENTER();

	if (WARN_ON(epfile->ffs->state != FFS_ACTIVE))
		return -ENODEV;

	spin_lock_irq(&epfile->ffs->eps_lock);
	if (likely(epfile->ep)) {
		switch (code) {
		case FUNCTIONFS_FIFO_STATUS:
			ret = usb_ep_fifo_status(epfile->ep->ep);
			break;
		case FUNCTIONFS_FIFO_FLUSH:
			usb_ep_fifo_flush(epfile->ep->ep);
			ret = 0;
			break;
		case FUNCTIONFS_CLEAR_HALT:
			ret = usb_ep_clear_halt(epfile->ep->ep);
			break;
		case FUNCTIONFS_ENDPOINT_REVMAP:
			ret = epfile->ep->num;
			break;
		case FUNCTIONFS_ENDPOINT_DESC:
		{
			int desc_idx;
			struct usb_endpoint_descriptor *desc;

			switch (epfile->ffs->gadget->speed) {
			case USB_SPEED_SUPER:
				desc_idx = 2;
				break;
			case USB_SPEED_HIGH:
				desc_idx = 1;
				break;
			default:
				desc_idx = 0;
			}
			desc = epfile->ep->descs[desc_idx];

			spin_unlock_irq(&epfile->ffs->eps_lock);
			ret = copy_to_user((void *)value, desc, sizeof(*desc));
			if (ret)
				ret = -EFAULT;
			return ret;
		}
		default:
			ret = -ENOTTY;
		}
	} else {
		ret = -ENODEV;
	}
	spin_unlock_irq(&epfile->ffs->eps_lock);

	return ret;
}
