static int arcmsr_hbaB_handle_isr(struct AdapterControlBlock *acb)
{
	uint32_t outbound_doorbell;
	struct MessageUnit_B *reg = acb->pmuB;
	outbound_doorbell = readl(reg->iop2drv_doorbell) &
				acb->outbound_int_enable;
	if (!outbound_doorbell)
		return IRQ_NONE;
	do {
		writel(~outbound_doorbell, reg->iop2drv_doorbell);
		writel(ARCMSR_DRV2IOP_END_OF_INTERRUPT, reg->drv2iop_doorbell);
		if (outbound_doorbell & ARCMSR_IOP2DRV_DATA_WRITE_OK)
			arcmsr_iop2drv_data_wrote_handle(acb);
		if (outbound_doorbell & ARCMSR_IOP2DRV_DATA_READ_OK)
			arcmsr_iop2drv_data_read_handle(acb);
		if (outbound_doorbell & ARCMSR_IOP2DRV_CDB_DONE)
			arcmsr_hbaB_postqueue_isr(acb);
		if (outbound_doorbell & ARCMSR_IOP2DRV_MESSAGE_CMD_DONE)
			arcmsr_hbaB_message_isr(acb);
		outbound_doorbell = readl(reg->iop2drv_doorbell) &
			acb->outbound_int_enable;
	} while (outbound_doorbell & (ARCMSR_IOP2DRV_DATA_WRITE_OK
		| ARCMSR_IOP2DRV_DATA_READ_OK
		| ARCMSR_IOP2DRV_CDB_DONE
		| ARCMSR_IOP2DRV_MESSAGE_CMD_DONE));
	return IRQ_HANDLED;
}
