static uint8_t arcmsr_hbaC_wait_msgint_ready(struct AdapterControlBlock *pACB)
{
	struct MessageUnit_C __iomem *phbcmu = pACB->pmuC;
	int i;

	for (i = 0; i < 2000; i++) {
		if (readl(&phbcmu->outbound_doorbell)
				& ARCMSR_HBCMU_IOP2DRV_MESSAGE_CMD_DONE) {
			writel(ARCMSR_HBCMU_IOP2DRV_MESSAGE_CMD_DONE_DOORBELL_CLEAR,
				&phbcmu->outbound_doorbell_clear); /*clear interrupt*/
			return true;
		}
		msleep(10);
	} /* max 20 seconds */

	return false;
}
