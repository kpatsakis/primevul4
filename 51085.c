static void arcmsr_hbaD_stop_bgrb(struct AdapterControlBlock *pACB)
{
	struct MessageUnit_D *reg = pACB->pmuD;

	pACB->acb_flags &= ~ACB_F_MSG_START_BGRB;
	writel(ARCMSR_INBOUND_MESG0_STOP_BGRB, reg->inbound_msgaddr0);
	if (!arcmsr_hbaD_wait_msgint_ready(pACB))
		pr_notice("arcmsr%d: wait 'stop adapter background rebulid' "
			"timeout\n", pACB->host->host_no);
}
