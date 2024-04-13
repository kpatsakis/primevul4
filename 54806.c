static void uas_do_work(struct work_struct *work)
{
	struct uas_dev_info *devinfo =
		container_of(work, struct uas_dev_info, work);
	struct uas_cmd_info *cmdinfo;
	struct scsi_cmnd *cmnd;
	unsigned long flags;
	int i, err;

	spin_lock_irqsave(&devinfo->lock, flags);

	if (devinfo->resetting)
		goto out;

	for (i = 0; i < devinfo->qdepth; i++) {
		if (!devinfo->cmnd[i])
			continue;

		cmnd = devinfo->cmnd[i];
		cmdinfo = (void *)&cmnd->SCp;

		if (!(cmdinfo->state & IS_IN_WORK_LIST))
			continue;

		err = uas_submit_urbs(cmnd, cmnd->device->hostdata);
		if (!err)
			cmdinfo->state &= ~IS_IN_WORK_LIST;
		else
			schedule_work(&devinfo->work);
	}
out:
	spin_unlock_irqrestore(&devinfo->lock, flags);
}
