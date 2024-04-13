static void arcmsr_hbaA_message_isr(struct AdapterControlBlock *acb)
{
	struct MessageUnit_A __iomem *reg  = acb->pmuA;
	/*clear interrupt and message state*/
	writel(ARCMSR_MU_OUTBOUND_MESSAGE0_INT, &reg->outbound_intstatus);
	schedule_work(&acb->arcmsr_do_message_isr_bh);
}
