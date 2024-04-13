static int uas_queuecommand_lck(struct scsi_cmnd *cmnd,
					void (*done)(struct scsi_cmnd *))
{
	struct scsi_device *sdev = cmnd->device;
	struct uas_dev_info *devinfo = sdev->hostdata;
	struct uas_cmd_info *cmdinfo = (void *)&cmnd->SCp;
	unsigned long flags;
	int idx, err;

	BUILD_BUG_ON(sizeof(struct uas_cmd_info) > sizeof(struct scsi_pointer));

	/* Re-check scsi_block_requests now that we've the host-lock */
	if (cmnd->device->host->host_self_blocked)
		return SCSI_MLQUEUE_DEVICE_BUSY;

	if ((devinfo->flags & US_FL_NO_ATA_1X) &&
			(cmnd->cmnd[0] == ATA_12 || cmnd->cmnd[0] == ATA_16)) {
		memcpy(cmnd->sense_buffer, usb_stor_sense_invalidCDB,
		       sizeof(usb_stor_sense_invalidCDB));
		cmnd->result = SAM_STAT_CHECK_CONDITION;
		cmnd->scsi_done(cmnd);
		return 0;
	}

	spin_lock_irqsave(&devinfo->lock, flags);

	if (devinfo->resetting) {
		cmnd->result = DID_ERROR << 16;
		cmnd->scsi_done(cmnd);
		spin_unlock_irqrestore(&devinfo->lock, flags);
		return 0;
	}

	/* Find a free uas-tag */
	for (idx = 0; idx < devinfo->qdepth; idx++) {
		if (!devinfo->cmnd[idx])
			break;
	}
	if (idx == devinfo->qdepth) {
		spin_unlock_irqrestore(&devinfo->lock, flags);
		return SCSI_MLQUEUE_DEVICE_BUSY;
	}

	cmnd->scsi_done = done;

	memset(cmdinfo, 0, sizeof(*cmdinfo));
	cmdinfo->uas_tag = idx + 1; /* uas-tag == usb-stream-id, so 1 based */
	cmdinfo->state = SUBMIT_STATUS_URB | ALLOC_CMD_URB | SUBMIT_CMD_URB;

	switch (cmnd->sc_data_direction) {
	case DMA_FROM_DEVICE:
		cmdinfo->state |= ALLOC_DATA_IN_URB | SUBMIT_DATA_IN_URB;
		break;
	case DMA_BIDIRECTIONAL:
		cmdinfo->state |= ALLOC_DATA_IN_URB | SUBMIT_DATA_IN_URB;
	case DMA_TO_DEVICE:
		cmdinfo->state |= ALLOC_DATA_OUT_URB | SUBMIT_DATA_OUT_URB;
	case DMA_NONE:
		break;
	}

	if (!devinfo->use_streams)
		cmdinfo->state &= ~(SUBMIT_DATA_IN_URB | SUBMIT_DATA_OUT_URB);

	err = uas_submit_urbs(cmnd, devinfo);
	if (err) {
		/* If we did nothing, give up now */
		if (cmdinfo->state & SUBMIT_STATUS_URB) {
			spin_unlock_irqrestore(&devinfo->lock, flags);
			return SCSI_MLQUEUE_DEVICE_BUSY;
		}
		uas_add_work(cmdinfo);
	}

	devinfo->cmnd[idx] = cmnd;
	spin_unlock_irqrestore(&devinfo->lock, flags);
	return 0;
}
