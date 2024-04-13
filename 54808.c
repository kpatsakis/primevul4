static int uas_eh_device_reset_handler(struct scsi_cmnd *cmnd)
{
	struct scsi_device *sdev = cmnd->device;
	struct uas_dev_info *devinfo = sdev->hostdata;
	struct usb_device *udev = devinfo->udev;
	unsigned long flags;
	int err;

	err = usb_lock_device_for_reset(udev, devinfo->intf);
	if (err) {
		shost_printk(KERN_ERR, sdev->host,
			     "%s FAILED to get lock err %d\n", __func__, err);
		return FAILED;
	}

	shost_printk(KERN_INFO, sdev->host, "%s start\n", __func__);

	spin_lock_irqsave(&devinfo->lock, flags);
	devinfo->resetting = 1;
	spin_unlock_irqrestore(&devinfo->lock, flags);

	usb_kill_anchored_urbs(&devinfo->cmd_urbs);
	usb_kill_anchored_urbs(&devinfo->sense_urbs);
	usb_kill_anchored_urbs(&devinfo->data_urbs);
	uas_zap_pending(devinfo, DID_RESET);

	err = usb_reset_device(udev);

	spin_lock_irqsave(&devinfo->lock, flags);
	devinfo->resetting = 0;
	spin_unlock_irqrestore(&devinfo->lock, flags);

	usb_unlock_device(udev);

	if (err) {
		shost_printk(KERN_INFO, sdev->host, "%s FAILED err %d\n",
			     __func__, err);
		return FAILED;
	}

	shost_printk(KERN_INFO, sdev->host, "%s success\n", __func__);
	return SUCCESS;
}
