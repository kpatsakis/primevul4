static uint8_t arcmsr_hbaB_wait_msgint_ready(struct AdapterControlBlock *acb)
{
	struct MessageUnit_B *reg = acb->pmuB;
	int i;

	for (i = 0; i < 2000; i++) {
		if (readl(reg->iop2drv_doorbell)
			& ARCMSR_IOP2DRV_MESSAGE_CMD_DONE) {
			writel(ARCMSR_MESSAGE_INT_CLEAR_PATTERN,
					reg->iop2drv_doorbell);
			writel(ARCMSR_DRV2IOP_END_OF_INTERRUPT,
					reg->drv2iop_doorbell);
			return true;
		}
		msleep(10);
	} /* max 20 seconds */

	return false;
}
