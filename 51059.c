static void arcmsr_hbaB_message_isr(struct AdapterControlBlock *acb)
{
	struct MessageUnit_B *reg  = acb->pmuB;

	/*clear interrupt and message state*/
	writel(ARCMSR_MESSAGE_INT_CLEAR_PATTERN, reg->iop2drv_doorbell);
	schedule_work(&acb->arcmsr_do_message_isr_bh);
}
