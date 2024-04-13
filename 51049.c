static int arcmsr_hbaA_handle_isr(struct AdapterControlBlock *acb)
{
	uint32_t outbound_intstatus;
	struct MessageUnit_A __iomem *reg = acb->pmuA;
	outbound_intstatus = readl(&reg->outbound_intstatus) &
		acb->outbound_int_enable;
	if (!(outbound_intstatus & ARCMSR_MU_OUTBOUND_HANDLE_INT))
		return IRQ_NONE;
	do {
		writel(outbound_intstatus, &reg->outbound_intstatus);
		if (outbound_intstatus & ARCMSR_MU_OUTBOUND_DOORBELL_INT)
			arcmsr_hbaA_doorbell_isr(acb);
		if (outbound_intstatus & ARCMSR_MU_OUTBOUND_POSTQUEUE_INT)
			arcmsr_hbaA_postqueue_isr(acb);
		if (outbound_intstatus & ARCMSR_MU_OUTBOUND_MESSAGE0_INT)
			arcmsr_hbaA_message_isr(acb);
		outbound_intstatus = readl(&reg->outbound_intstatus) &
			acb->outbound_int_enable;
	} while (outbound_intstatus & (ARCMSR_MU_OUTBOUND_DOORBELL_INT
		| ARCMSR_MU_OUTBOUND_POSTQUEUE_INT
		| ARCMSR_MU_OUTBOUND_MESSAGE0_INT));
	return IRQ_HANDLED;
}
