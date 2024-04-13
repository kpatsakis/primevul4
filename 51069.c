static void arcmsr_hbaC_message_isr(struct AdapterControlBlock *acb)
{
	struct MessageUnit_C __iomem *reg  = acb->pmuC;
	/*clear interrupt and message state*/
	writel(ARCMSR_HBCMU_IOP2DRV_MESSAGE_CMD_DONE_DOORBELL_CLEAR, &reg->outbound_doorbell_clear);
	schedule_work(&acb->arcmsr_do_message_isr_bh);
}
