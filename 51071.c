static void arcmsr_hbaC_postqueue_isr(struct AdapterControlBlock *acb)
{
	struct MessageUnit_C __iomem *phbcmu;
	struct ARCMSR_CDB *arcmsr_cdb;
	struct CommandControlBlock *ccb;
	uint32_t flag_ccb, ccb_cdb_phy, throttling = 0;
	int error;

	phbcmu = acb->pmuC;
	/* areca cdb command done */
	/* Use correct offset and size for syncing */

	while ((flag_ccb = readl(&phbcmu->outbound_queueport_low)) !=
			0xFFFFFFFF) {
		ccb_cdb_phy = (flag_ccb & 0xFFFFFFF0);
		arcmsr_cdb = (struct ARCMSR_CDB *)(acb->vir2phy_offset
			+ ccb_cdb_phy);
		ccb = container_of(arcmsr_cdb, struct CommandControlBlock,
			arcmsr_cdb);
		error = (flag_ccb & ARCMSR_CCBREPLY_FLAG_ERROR_MODE1)
			? true : false;
		/* check if command done with no error */
		arcmsr_drain_donequeue(acb, ccb, error);
		throttling++;
		if (throttling == ARCMSR_HBC_ISR_THROTTLING_LEVEL) {
			writel(ARCMSR_HBCMU_DRV2IOP_POSTQUEUE_THROTTLING,
				&phbcmu->inbound_doorbell);
			throttling = 0;
		}
	}
}
