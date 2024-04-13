static int arcmsr_queue_command_lck(struct scsi_cmnd *cmd,
	void (* done)(struct scsi_cmnd *))
{
	struct Scsi_Host *host = cmd->device->host;
	struct AdapterControlBlock *acb = (struct AdapterControlBlock *) host->hostdata;
	struct CommandControlBlock *ccb;
	int target = cmd->device->id;
	int lun = cmd->device->lun;
	uint8_t scsicmd = cmd->cmnd[0];
	cmd->scsi_done = done;
	cmd->host_scribble = NULL;
	cmd->result = 0;
	if ((scsicmd == SYNCHRONIZE_CACHE) ||(scsicmd == SEND_DIAGNOSTIC)){
		if(acb->devstate[target][lun] == ARECA_RAID_GONE) {
    			cmd->result = (DID_NO_CONNECT << 16);
		}
		cmd->scsi_done(cmd);
		return 0;
	}
	if (target == 16) {
		/* virtual device for iop message transfer */
		arcmsr_handle_virtual_command(acb, cmd);
		return 0;
	}
	ccb = arcmsr_get_freeccb(acb);
	if (!ccb)
		return SCSI_MLQUEUE_HOST_BUSY;
	if (arcmsr_build_ccb( acb, ccb, cmd ) == FAILED) {
		cmd->result = (DID_ERROR << 16) | (RESERVATION_CONFLICT << 1);
		cmd->scsi_done(cmd);
		return 0;
	}
	arcmsr_post_ccb(acb, ccb);
	return 0;
}
