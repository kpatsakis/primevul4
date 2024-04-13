static void uas_data_cmplt(struct urb *urb)
{
	struct scsi_cmnd *cmnd = urb->context;
	struct uas_cmd_info *cmdinfo = (void *)&cmnd->SCp;
	struct uas_dev_info *devinfo = (void *)cmnd->device->hostdata;
	struct scsi_data_buffer *sdb = NULL;
	unsigned long flags;
	int status = urb->status;

	spin_lock_irqsave(&devinfo->lock, flags);

	if (cmdinfo->data_in_urb == urb) {
		sdb = scsi_in(cmnd);
		cmdinfo->state &= ~DATA_IN_URB_INFLIGHT;
		cmdinfo->data_in_urb = NULL;
	} else if (cmdinfo->data_out_urb == urb) {
		sdb = scsi_out(cmnd);
		cmdinfo->state &= ~DATA_OUT_URB_INFLIGHT;
		cmdinfo->data_out_urb = NULL;
	}
	if (sdb == NULL) {
		WARN_ON_ONCE(1);
		goto out;
	}

	if (devinfo->resetting)
		goto out;

	/* Data urbs should not complete before the cmd urb is submitted */
	if (cmdinfo->state & SUBMIT_CMD_URB) {
		uas_log_cmd_state(cmnd, "unexpected data cmplt", 0);
		goto out;
	}

	if (status) {
		if (status != -ENOENT && status != -ECONNRESET && status != -ESHUTDOWN)
			uas_log_cmd_state(cmnd, "data cmplt err", status);
		/* error: no data transfered */
		sdb->resid = sdb->length;
	} else {
		sdb->resid = sdb->length - urb->actual_length;
	}
	uas_try_complete(cmnd, __func__);
out:
	usb_free_urb(urb);
	spin_unlock_irqrestore(&devinfo->lock, flags);
}
