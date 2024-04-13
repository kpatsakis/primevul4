static uint8_t arcmsr_hbaD_abort_allcmd(struct AdapterControlBlock *pACB)
{
	struct MessageUnit_D *reg = pACB->pmuD;

	writel(ARCMSR_INBOUND_MESG0_ABORT_CMD, reg->inbound_msgaddr0);
	if (!arcmsr_hbaD_wait_msgint_ready(pACB)) {
		pr_notice("arcmsr%d: wait 'abort all outstanding "
			"command' timeout\n", pACB->host->host_no);
		return false;
	}
	return true;
}
