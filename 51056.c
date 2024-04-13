static void arcmsr_hbaB_flush_cache(struct AdapterControlBlock *acb)
{
	struct MessageUnit_B *reg = acb->pmuB;
	int retry_count = 30;
	writel(ARCMSR_MESSAGE_FLUSH_CACHE, reg->drv2iop_doorbell);
	do {
		if (arcmsr_hbaB_wait_msgint_ready(acb))
			break;
		else {
			retry_count--;
			printk(KERN_NOTICE "arcmsr%d: wait 'flush adapter cache' \
			timeout,retry count down = %d \n", acb->host->host_no, retry_count);
		}
	} while (retry_count != 0);
}
