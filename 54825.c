static int uas_try_complete(struct scsi_cmnd *cmnd, const char *caller)
{
	struct uas_cmd_info *cmdinfo = (void *)&cmnd->SCp;
	struct uas_dev_info *devinfo = (void *)cmnd->device->hostdata;

	lockdep_assert_held(&devinfo->lock);
	if (cmdinfo->state & (COMMAND_INFLIGHT |
			      DATA_IN_URB_INFLIGHT |
			      DATA_OUT_URB_INFLIGHT |
			      COMMAND_ABORTED))
		return -EBUSY;
	devinfo->cmnd[cmdinfo->uas_tag - 1] = NULL;
	uas_free_unsubmitted_urbs(cmnd);
	cmnd->scsi_done(cmnd);
	return 0;
}
