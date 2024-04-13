static int uas_eh_abort_handler(struct scsi_cmnd *cmnd)
{
	struct uas_cmd_info *cmdinfo = (void *)&cmnd->SCp;
	struct uas_dev_info *devinfo = (void *)cmnd->device->hostdata;
	struct urb *data_in_urb = NULL;
	struct urb *data_out_urb = NULL;
	unsigned long flags;

	spin_lock_irqsave(&devinfo->lock, flags);

	uas_log_cmd_state(cmnd, __func__, 0);

	/* Ensure that try_complete does not call scsi_done */
	cmdinfo->state |= COMMAND_ABORTED;

	/* Drop all refs to this cmnd, kill data urbs to break their ref */
	devinfo->cmnd[cmdinfo->uas_tag - 1] = NULL;
	if (cmdinfo->state & DATA_IN_URB_INFLIGHT)
		data_in_urb = usb_get_urb(cmdinfo->data_in_urb);
	if (cmdinfo->state & DATA_OUT_URB_INFLIGHT)
		data_out_urb = usb_get_urb(cmdinfo->data_out_urb);

	uas_free_unsubmitted_urbs(cmnd);

	spin_unlock_irqrestore(&devinfo->lock, flags);

	if (data_in_urb) {
		usb_kill_urb(data_in_urb);
		usb_put_urb(data_in_urb);
	}
	if (data_out_urb) {
		usb_kill_urb(data_out_urb);
		usb_put_urb(data_out_urb);
	}

	return FAILED;
}
