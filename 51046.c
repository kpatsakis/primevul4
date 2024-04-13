static void arcmsr_hbaA_doorbell_isr(struct AdapterControlBlock *acb)
{
	uint32_t outbound_doorbell;
	struct MessageUnit_A __iomem *reg = acb->pmuA;
	outbound_doorbell = readl(&reg->outbound_doorbell);
	do {
		writel(outbound_doorbell, &reg->outbound_doorbell);
		if (outbound_doorbell & ARCMSR_OUTBOUND_IOP331_DATA_WRITE_OK)
			arcmsr_iop2drv_data_wrote_handle(acb);
		if (outbound_doorbell & ARCMSR_OUTBOUND_IOP331_DATA_READ_OK)
			arcmsr_iop2drv_data_read_handle(acb);
		outbound_doorbell = readl(&reg->outbound_doorbell);
	} while (outbound_doorbell & (ARCMSR_OUTBOUND_IOP331_DATA_WRITE_OK
		| ARCMSR_OUTBOUND_IOP331_DATA_READ_OK));
}
