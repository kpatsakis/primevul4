static void arcmsr_hbaD_message_isr(struct AdapterControlBlock *acb)
{
	struct MessageUnit_D *reg  = acb->pmuD;

	writel(ARCMSR_ARC1214_IOP2DRV_MESSAGE_CMD_DONE, reg->outbound_doorbell);
	readl(reg->outbound_doorbell);
	schedule_work(&acb->arcmsr_do_message_isr_bh);
}
