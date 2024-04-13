static void arcmsr_hbaD_postqueue_isr(struct AdapterControlBlock *acb)
{
	u32 outbound_write_pointer, doneq_index, index_stripped, toggle;
	uint32_t addressLow, ccb_cdb_phy;
	int error;
	struct MessageUnit_D  *pmu;
	struct ARCMSR_CDB *arcmsr_cdb;
	struct CommandControlBlock *ccb;
	unsigned long flags;

	spin_lock_irqsave(&acb->doneq_lock, flags);
	pmu = acb->pmuD;
	outbound_write_pointer = pmu->done_qbuffer[0].addressLow + 1;
	doneq_index = pmu->doneq_index;
	if ((doneq_index & 0xFFF) != (outbound_write_pointer & 0xFFF)) {
		do {
			toggle = doneq_index & 0x4000;
			index_stripped = (doneq_index & 0xFFF) + 1;
			index_stripped %= ARCMSR_MAX_ARC1214_DONEQUEUE;
			pmu->doneq_index = index_stripped ? (index_stripped | toggle) :
				((toggle ^ 0x4000) + 1);
			doneq_index = pmu->doneq_index;
			addressLow = pmu->done_qbuffer[doneq_index &
				0xFFF].addressLow;
			ccb_cdb_phy = (addressLow & 0xFFFFFFF0);
			arcmsr_cdb = (struct ARCMSR_CDB *)(acb->vir2phy_offset
				+ ccb_cdb_phy);
			ccb = container_of(arcmsr_cdb,
				struct CommandControlBlock, arcmsr_cdb);
			error = (addressLow & ARCMSR_CCBREPLY_FLAG_ERROR_MODE1)
				? true : false;
			arcmsr_drain_donequeue(acb, ccb, error);
			writel(doneq_index, pmu->outboundlist_read_pointer);
		} while ((doneq_index & 0xFFF) !=
			(outbound_write_pointer & 0xFFF));
	}
	writel(ARCMSR_ARC1214_OUTBOUND_LIST_INTERRUPT_CLEAR,
		pmu->outboundlist_interrupt_cause);
	readl(pmu->outboundlist_interrupt_cause);
	spin_unlock_irqrestore(&acb->doneq_lock, flags);
}
