static int arcmsr_hbaD_polling_ccbdone(struct AdapterControlBlock *acb,
				struct CommandControlBlock *poll_ccb)
{
	bool error;
	uint32_t poll_ccb_done = 0, poll_count = 0, flag_ccb, ccb_cdb_phy;
	int rtn, doneq_index, index_stripped, outbound_write_pointer, toggle;
	unsigned long flags;
	struct ARCMSR_CDB *arcmsr_cdb;
	struct CommandControlBlock *pCCB;
	struct MessageUnit_D *pmu = acb->pmuD;

polling_hbaD_ccb_retry:
	poll_count++;
	while (1) {
		spin_lock_irqsave(&acb->doneq_lock, flags);
		outbound_write_pointer = pmu->done_qbuffer[0].addressLow + 1;
		doneq_index = pmu->doneq_index;
		if ((outbound_write_pointer & 0xFFF) == (doneq_index & 0xFFF)) {
			spin_unlock_irqrestore(&acb->doneq_lock, flags);
			if (poll_ccb_done) {
				rtn = SUCCESS;
				break;
			} else {
				msleep(25);
				if (poll_count > 40) {
					rtn = FAILED;
					break;
				}
				goto polling_hbaD_ccb_retry;
			}
		}
		toggle = doneq_index & 0x4000;
		index_stripped = (doneq_index & 0xFFF) + 1;
		index_stripped %= ARCMSR_MAX_ARC1214_DONEQUEUE;
		pmu->doneq_index = index_stripped ? (index_stripped | toggle) :
				((toggle ^ 0x4000) + 1);
		doneq_index = pmu->doneq_index;
		spin_unlock_irqrestore(&acb->doneq_lock, flags);
		flag_ccb = pmu->done_qbuffer[doneq_index & 0xFFF].addressLow;
		ccb_cdb_phy = (flag_ccb & 0xFFFFFFF0);
		arcmsr_cdb = (struct ARCMSR_CDB *)(acb->vir2phy_offset +
			ccb_cdb_phy);
		pCCB = container_of(arcmsr_cdb, struct CommandControlBlock,
			arcmsr_cdb);
		poll_ccb_done |= (pCCB == poll_ccb) ? 1 : 0;
		if ((pCCB->acb != acb) ||
			(pCCB->startdone != ARCMSR_CCB_START)) {
			if (pCCB->startdone == ARCMSR_CCB_ABORTED) {
				pr_notice("arcmsr%d: scsi id = %d "
					"lun = %d ccb = '0x%p' poll command "
					"abort successfully\n"
					, acb->host->host_no
					, pCCB->pcmd->device->id
					, (u32)pCCB->pcmd->device->lun
					, pCCB);
				pCCB->pcmd->result = DID_ABORT << 16;
				arcmsr_ccb_complete(pCCB);
				continue;
			}
			pr_notice("arcmsr%d: polling an illegal "
				"ccb command done ccb = '0x%p' "
				"ccboutstandingcount = %d\n"
				, acb->host->host_no
				, pCCB
				, atomic_read(&acb->ccboutstandingcount));
			continue;
		}
		error = (flag_ccb & ARCMSR_CCBREPLY_FLAG_ERROR_MODE1)
			? true : false;
		arcmsr_report_ccb_state(acb, pCCB, error);
	}
	return rtn;
}
