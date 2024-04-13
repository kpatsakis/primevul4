static uint8_t arcmsr_hbaC_abort_allcmd(struct AdapterControlBlock *pACB)
{
	struct MessageUnit_C __iomem *reg = pACB->pmuC;
	writel(ARCMSR_INBOUND_MESG0_ABORT_CMD, &reg->inbound_msgaddr0);
	writel(ARCMSR_HBCMU_DRV2IOP_MESSAGE_CMD_DONE, &reg->inbound_doorbell);
	if (!arcmsr_hbaC_wait_msgint_ready(pACB)) {
		printk(KERN_NOTICE
			"arcmsr%d: wait 'abort all outstanding command' timeout\n"
			, pACB->host->host_no);
		return false;
	}
	return true;
}
