static int arcmsr_abort(struct scsi_cmnd *cmd)
{
	struct AdapterControlBlock *acb =
		(struct AdapterControlBlock *)cmd->device->host->hostdata;
	int i = 0;
	int rtn = FAILED;
	uint32_t intmask_org;

	printk(KERN_NOTICE
		"arcmsr%d: abort device command of scsi id = %d lun = %d\n",
		acb->host->host_no, cmd->device->id, (u32)cmd->device->lun);
	acb->acb_flags |= ACB_F_ABORT;
	acb->num_aborts++;
	/*
	************************************************
	** the all interrupt service routine is locked
	** we need to handle it as soon as possible and exit
	************************************************
	*/
	if (!atomic_read(&acb->ccboutstandingcount)) {
		acb->acb_flags &= ~ACB_F_ABORT;
		return rtn;
	}

	intmask_org = arcmsr_disable_outbound_ints(acb);
	for (i = 0; i < ARCMSR_MAX_FREECCB_NUM; i++) {
		struct CommandControlBlock *ccb = acb->pccb_pool[i];
		if (ccb->startdone == ARCMSR_CCB_START && ccb->pcmd == cmd) {
			ccb->startdone = ARCMSR_CCB_ABORTED;
			rtn = arcmsr_abort_one_cmd(acb, ccb);
			break;
		}
	}
	acb->acb_flags &= ~ACB_F_ABORT;
	arcmsr_enable_outbound_ints(acb, intmask_org);
	return rtn;
}
