static int arcmsr_hbaB_polling_ccbdone(struct AdapterControlBlock *acb,
					struct CommandControlBlock *poll_ccb)
{
	struct MessageUnit_B *reg = acb->pmuB;
	struct ARCMSR_CDB *arcmsr_cdb;
	struct CommandControlBlock *ccb;
	uint32_t flag_ccb, poll_ccb_done = 0, poll_count = 0;
	int index, rtn;
	bool error;
	polling_hbb_ccb_retry:

	poll_count++;
	/* clear doorbell interrupt */
	writel(ARCMSR_DOORBELL_INT_CLEAR_PATTERN, reg->iop2drv_doorbell);
	while(1){
		index = reg->doneq_index;
		flag_ccb = reg->done_qbuffer[index];
		if (flag_ccb == 0) {
			if (poll_ccb_done){
				rtn = SUCCESS;
				break;
			}else {
				msleep(25);
				if (poll_count > 100){
					rtn = FAILED;
					break;
				}
				goto polling_hbb_ccb_retry;
			}
		}
		reg->done_qbuffer[index] = 0;
		index++;
		/*if last index number set it to 0 */
		index %= ARCMSR_MAX_HBB_POSTQUEUE;
		reg->doneq_index = index;
		/* check if command done with no error*/
		arcmsr_cdb = (struct ARCMSR_CDB *)(acb->vir2phy_offset + (flag_ccb << 5));
		ccb = container_of(arcmsr_cdb, struct CommandControlBlock, arcmsr_cdb);
		poll_ccb_done |= (ccb == poll_ccb) ? 1 : 0;
		if ((ccb->acb != acb) || (ccb->startdone != ARCMSR_CCB_START)) {
			if ((ccb->startdone == ARCMSR_CCB_ABORTED) || (ccb == poll_ccb)) {
				printk(KERN_NOTICE "arcmsr%d: scsi id = %d lun = %d ccb = '0x%p'"
					" poll command abort successfully \n"
					,acb->host->host_no
					,ccb->pcmd->device->id
					,(u32)ccb->pcmd->device->lun
					,ccb);
				ccb->pcmd->result = DID_ABORT << 16;
				arcmsr_ccb_complete(ccb);
				continue;
			}
			printk(KERN_NOTICE "arcmsr%d: polling get an illegal ccb"
				" command done ccb = '0x%p'"
				"ccboutstandingcount = %d \n"
				, acb->host->host_no
				, ccb
				, atomic_read(&acb->ccboutstandingcount));
			continue;
		} 
		error = (flag_ccb & ARCMSR_CCBREPLY_FLAG_ERROR_MODE0) ? true : false;
		arcmsr_report_ccb_state(acb, ccb, error);
	}
	return rtn;
}
