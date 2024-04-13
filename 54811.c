static void uas_free_unsubmitted_urbs(struct scsi_cmnd *cmnd)
{
	struct uas_cmd_info *cmdinfo;

	if (!cmnd)
		return;

	cmdinfo = (void *)&cmnd->SCp;

	if (cmdinfo->state & SUBMIT_CMD_URB)
		usb_free_urb(cmdinfo->cmd_urb);

	/* data urbs may have never gotten their submit flag set */
	if (!(cmdinfo->state & DATA_IN_URB_INFLIGHT))
		usb_free_urb(cmdinfo->data_in_urb);
	if (!(cmdinfo->state & DATA_OUT_URB_INFLIGHT))
		usb_free_urb(cmdinfo->data_out_urb);
}
