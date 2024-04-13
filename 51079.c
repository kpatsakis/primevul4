static irqreturn_t arcmsr_hbaD_handle_isr(struct AdapterControlBlock *pACB)
{
	u32 host_interrupt_status;
	struct MessageUnit_D  *pmu = pACB->pmuD;

	host_interrupt_status = readl(pmu->host_int_status) &
		(ARCMSR_ARC1214_OUTBOUND_POSTQUEUE_ISR |
		ARCMSR_ARC1214_OUTBOUND_DOORBELL_ISR);
	if (!host_interrupt_status)
		return IRQ_NONE;
	do {
		/* MU post queue interrupts*/
		if (host_interrupt_status &
			ARCMSR_ARC1214_OUTBOUND_POSTQUEUE_ISR)
			arcmsr_hbaD_postqueue_isr(pACB);
		if (host_interrupt_status &
			ARCMSR_ARC1214_OUTBOUND_DOORBELL_ISR)
			arcmsr_hbaD_doorbell_isr(pACB);
		host_interrupt_status = readl(pmu->host_int_status);
	} while (host_interrupt_status &
		(ARCMSR_ARC1214_OUTBOUND_POSTQUEUE_ISR |
		ARCMSR_ARC1214_OUTBOUND_DOORBELL_ISR));
	return IRQ_HANDLED;
}
