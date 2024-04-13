static void uas_zap_pending(struct uas_dev_info *devinfo, int result)
{
	struct uas_cmd_info *cmdinfo;
	struct scsi_cmnd *cmnd;
	unsigned long flags;
	int i, err;

	spin_lock_irqsave(&devinfo->lock, flags);
	for (i = 0; i < devinfo->qdepth; i++) {
		if (!devinfo->cmnd[i])
			continue;

		cmnd = devinfo->cmnd[i];
		cmdinfo = (void *)&cmnd->SCp;
		uas_log_cmd_state(cmnd, __func__, 0);
		/* Sense urbs were killed, clear COMMAND_INFLIGHT manually */
		cmdinfo->state &= ~COMMAND_INFLIGHT;
		cmnd->result = result << 16;
		err = uas_try_complete(cmnd, __func__);
		WARN_ON(err != 0);
	}
	spin_unlock_irqrestore(&devinfo->lock, flags);
}
