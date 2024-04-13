static void uas_stat_cmplt(struct urb *urb)
{
	struct iu *iu = urb->transfer_buffer;
	struct Scsi_Host *shost = urb->context;
	struct uas_dev_info *devinfo = (struct uas_dev_info *)shost->hostdata;
	struct urb *data_in_urb = NULL;
	struct urb *data_out_urb = NULL;
	struct scsi_cmnd *cmnd;
	struct uas_cmd_info *cmdinfo;
	unsigned long flags;
	unsigned int idx;
	int status = urb->status;
	bool success;

	spin_lock_irqsave(&devinfo->lock, flags);

	if (devinfo->resetting)
		goto out;

	if (status) {
		if (status != -ENOENT && status != -ECONNRESET && status != -ESHUTDOWN)
			dev_err(&urb->dev->dev, "stat urb: status %d\n", status);
		goto out;
	}

	idx = be16_to_cpup(&iu->tag) - 1;
	if (idx >= MAX_CMNDS || !devinfo->cmnd[idx]) {
		dev_err(&urb->dev->dev,
			"stat urb: no pending cmd for uas-tag %d\n", idx + 1);
		goto out;
	}

	cmnd = devinfo->cmnd[idx];
	cmdinfo = (void *)&cmnd->SCp;

	if (!(cmdinfo->state & COMMAND_INFLIGHT)) {
		uas_log_cmd_state(cmnd, "unexpected status cmplt", 0);
		goto out;
	}

	switch (iu->iu_id) {
	case IU_ID_STATUS:
		uas_sense(urb, cmnd);
		if (cmnd->result != 0) {
			/* cancel data transfers on error */
			data_in_urb = usb_get_urb(cmdinfo->data_in_urb);
			data_out_urb = usb_get_urb(cmdinfo->data_out_urb);
		}
		cmdinfo->state &= ~COMMAND_INFLIGHT;
		uas_try_complete(cmnd, __func__);
		break;
	case IU_ID_READ_READY:
		if (!cmdinfo->data_in_urb ||
				(cmdinfo->state & DATA_IN_URB_INFLIGHT)) {
			uas_log_cmd_state(cmnd, "unexpected read rdy", 0);
			break;
		}
		uas_xfer_data(urb, cmnd, SUBMIT_DATA_IN_URB);
		break;
	case IU_ID_WRITE_READY:
		if (!cmdinfo->data_out_urb ||
				(cmdinfo->state & DATA_OUT_URB_INFLIGHT)) {
			uas_log_cmd_state(cmnd, "unexpected write rdy", 0);
			break;
		}
		uas_xfer_data(urb, cmnd, SUBMIT_DATA_OUT_URB);
		break;
	case IU_ID_RESPONSE:
		cmdinfo->state &= ~COMMAND_INFLIGHT;
		success = uas_evaluate_response_iu((struct response_iu *)iu, cmnd);
		if (!success) {
			/* Error, cancel data transfers */
			data_in_urb = usb_get_urb(cmdinfo->data_in_urb);
			data_out_urb = usb_get_urb(cmdinfo->data_out_urb);
		}
		uas_try_complete(cmnd, __func__);
		break;
	default:
		uas_log_cmd_state(cmnd, "bogus IU", iu->iu_id);
	}
out:
	usb_free_urb(urb);
	spin_unlock_irqrestore(&devinfo->lock, flags);

	/* Unlinking of data urbs must be done without holding the lock */
	if (data_in_urb) {
		usb_unlink_urb(data_in_urb);
		usb_put_urb(data_in_urb);
	}
	if (data_out_urb) {
		usb_unlink_urb(data_out_urb);
		usb_put_urb(data_out_urb);
	}
}
