static int uas_pre_reset(struct usb_interface *intf)
{
	struct Scsi_Host *shost = usb_get_intfdata(intf);
	struct uas_dev_info *devinfo = (struct uas_dev_info *)shost->hostdata;
	unsigned long flags;

	if (devinfo->shutdown)
		return 0;

	/* Block new requests */
	spin_lock_irqsave(shost->host_lock, flags);
	scsi_block_requests(shost);
	spin_unlock_irqrestore(shost->host_lock, flags);

	if (uas_wait_for_pending_cmnds(devinfo) != 0) {
		shost_printk(KERN_ERR, shost, "%s: timed out\n", __func__);
		scsi_unblock_requests(shost);
		return 1;
	}

	uas_free_streams(devinfo);

	return 0;
}
