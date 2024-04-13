static void arcmsr_hbaD_flush_cache(struct AdapterControlBlock *pACB)
{
	int retry_count = 15;
	struct MessageUnit_D *reg = pACB->pmuD;

	writel(ARCMSR_INBOUND_MESG0_FLUSH_CACHE, reg->inbound_msgaddr0);
	do {
		if (arcmsr_hbaD_wait_msgint_ready(pACB))
			break;

		retry_count--;
		pr_notice("arcmsr%d: wait 'flush adapter "
			"cache' timeout, retry count down = %d\n",
			pACB->host->host_no, retry_count);
	} while (retry_count != 0);
}
