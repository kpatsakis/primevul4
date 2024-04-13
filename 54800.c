static struct urb *uas_alloc_sense_urb(struct uas_dev_info *devinfo, gfp_t gfp,
				       struct scsi_cmnd *cmnd)
{
	struct usb_device *udev = devinfo->udev;
	struct uas_cmd_info *cmdinfo = (void *)&cmnd->SCp;
	struct urb *urb = usb_alloc_urb(0, gfp);
	struct sense_iu *iu;

	if (!urb)
		goto out;

	iu = kzalloc(sizeof(*iu), gfp);
	if (!iu)
		goto free;

	usb_fill_bulk_urb(urb, udev, devinfo->status_pipe, iu, sizeof(*iu),
			  uas_stat_cmplt, cmnd->device->host);
	if (devinfo->use_streams)
		urb->stream_id = cmdinfo->uas_tag;
	urb->transfer_flags |= URB_FREE_BUFFER;
 out:
	return urb;
 free:
	usb_free_urb(urb);
	return NULL;
}
