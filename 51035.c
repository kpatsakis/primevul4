static void arcmsr_drain_donequeue(struct AdapterControlBlock *acb, struct CommandControlBlock *pCCB, bool error)
{
	int id, lun;
	if ((pCCB->acb != acb) || (pCCB->startdone != ARCMSR_CCB_START)) {
		if (pCCB->startdone == ARCMSR_CCB_ABORTED) {
			struct scsi_cmnd *abortcmd = pCCB->pcmd;
			if (abortcmd) {
				id = abortcmd->device->id;
				lun = abortcmd->device->lun;				
				abortcmd->result |= DID_ABORT << 16;
				arcmsr_ccb_complete(pCCB);
				printk(KERN_NOTICE "arcmsr%d: pCCB ='0x%p' isr got aborted command \n",
				acb->host->host_no, pCCB);
			}
			return;
		}
		printk(KERN_NOTICE "arcmsr%d: isr get an illegal ccb command \
				done acb = '0x%p'"
				"ccb = '0x%p' ccbacb = '0x%p' startdone = 0x%x"
				" ccboutstandingcount = %d \n"
				, acb->host->host_no
				, acb
				, pCCB
				, pCCB->acb
				, pCCB->startdone
				, atomic_read(&acb->ccboutstandingcount));
		  return;
	}
	arcmsr_report_ccb_state(acb, pCCB, error);
}
