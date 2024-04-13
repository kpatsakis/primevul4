static int arcmsr_hbaC_handle_isr(struct AdapterControlBlock *pACB)
{
	uint32_t host_interrupt_status;
	struct MessageUnit_C __iomem *phbcmu = pACB->pmuC;
	/*
	*********************************************
	**   check outbound intstatus
	*********************************************
	*/
	host_interrupt_status = readl(&phbcmu->host_int_status) &
		(ARCMSR_HBCMU_OUTBOUND_POSTQUEUE_ISR |
		ARCMSR_HBCMU_OUTBOUND_DOORBELL_ISR);
	if (!host_interrupt_status)
		return IRQ_NONE;
	do {
		if (host_interrupt_status & ARCMSR_HBCMU_OUTBOUND_DOORBELL_ISR)
			arcmsr_hbaC_doorbell_isr(pACB);
		/* MU post queue interrupts*/
		if (host_interrupt_status & ARCMSR_HBCMU_OUTBOUND_POSTQUEUE_ISR)
			arcmsr_hbaC_postqueue_isr(pACB);
		host_interrupt_status = readl(&phbcmu->host_int_status);
	} while (host_interrupt_status & (ARCMSR_HBCMU_OUTBOUND_POSTQUEUE_ISR |
		ARCMSR_HBCMU_OUTBOUND_DOORBELL_ISR));
	return IRQ_HANDLED;
}
