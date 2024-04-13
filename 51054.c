static uint8_t arcmsr_hbaA_wait_msgint_ready(struct AdapterControlBlock *acb)
{
	struct MessageUnit_A __iomem *reg = acb->pmuA;
	int i;

	for (i = 0; i < 2000; i++) {
		if (readl(&reg->outbound_intstatus) &
				ARCMSR_MU_OUTBOUND_MESSAGE0_INT) {
			writel(ARCMSR_MU_OUTBOUND_MESSAGE0_INT,
				&reg->outbound_intstatus);
			return true;
		}
		msleep(10);
	} /* max 20 seconds */

	return false;
}
