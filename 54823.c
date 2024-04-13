static int uas_suspend(struct usb_interface *intf, pm_message_t message)
{
	struct Scsi_Host *shost = usb_get_intfdata(intf);
	struct uas_dev_info *devinfo = (struct uas_dev_info *)shost->hostdata;

	if (uas_wait_for_pending_cmnds(devinfo) != 0) {
		shost_printk(KERN_ERR, shost, "%s: timed out\n", __func__);
		return -ETIME;
	}

	return 0;
}
