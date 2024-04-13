static void arcmsr_hbaC_flush_cache(struct AdapterControlBlock *pACB)
{
	struct MessageUnit_C __iomem *reg = pACB->pmuC;
	int retry_count = 30;/* enlarge wait flush adapter cache time: 10 minute */
	writel(ARCMSR_INBOUND_MESG0_FLUSH_CACHE, &reg->inbound_msgaddr0);
	writel(ARCMSR_HBCMU_DRV2IOP_MESSAGE_CMD_DONE, &reg->inbound_doorbell);
	do {
		if (arcmsr_hbaC_wait_msgint_ready(pACB)) {
			break;
		} else {
			retry_count--;
			printk(KERN_NOTICE "arcmsr%d: wait 'flush adapter cache' \
			timeout,retry count down = %d \n", pACB->host->host_no, retry_count);
		}
	} while (retry_count != 0);
	return;
}
