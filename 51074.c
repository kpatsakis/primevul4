static void arcmsr_hbaC_stop_bgrb(struct AdapterControlBlock *pACB)
{
	struct MessageUnit_C __iomem *reg = pACB->pmuC;
	pACB->acb_flags &= ~ACB_F_MSG_START_BGRB;
	writel(ARCMSR_INBOUND_MESG0_STOP_BGRB, &reg->inbound_msgaddr0);
	writel(ARCMSR_HBCMU_DRV2IOP_MESSAGE_CMD_DONE, &reg->inbound_doorbell);
	if (!arcmsr_hbaC_wait_msgint_ready(pACB)) {
		printk(KERN_NOTICE
			"arcmsr%d: wait 'stop adapter background rebulid' timeout\n"
			, pACB->host->host_no);
	}
	return;
}
