static void arcmsr_enable_outbound_ints(struct AdapterControlBlock *acb,
						u32 intmask_org)
{
	u32 mask;
	switch (acb->adapter_type) {

	case ACB_ADAPTER_TYPE_A: {
		struct MessageUnit_A __iomem *reg = acb->pmuA;
		mask = intmask_org & ~(ARCMSR_MU_OUTBOUND_POSTQUEUE_INTMASKENABLE |
			     ARCMSR_MU_OUTBOUND_DOORBELL_INTMASKENABLE|
			     ARCMSR_MU_OUTBOUND_MESSAGE0_INTMASKENABLE);
		writel(mask, &reg->outbound_intmask);
		acb->outbound_int_enable = ~(intmask_org & mask) & 0x000000ff;
		}
		break;

	case ACB_ADAPTER_TYPE_B: {
		struct MessageUnit_B *reg = acb->pmuB;
		mask = intmask_org | (ARCMSR_IOP2DRV_DATA_WRITE_OK |
			ARCMSR_IOP2DRV_DATA_READ_OK |
			ARCMSR_IOP2DRV_CDB_DONE |
			ARCMSR_IOP2DRV_MESSAGE_CMD_DONE);
		writel(mask, reg->iop2drv_doorbell_mask);
		acb->outbound_int_enable = (intmask_org | mask) & 0x0000000f;
		}
		break;
	case ACB_ADAPTER_TYPE_C: {
		struct MessageUnit_C __iomem *reg = acb->pmuC;
		mask = ~(ARCMSR_HBCMU_UTILITY_A_ISR_MASK | ARCMSR_HBCMU_OUTBOUND_DOORBELL_ISR_MASK|ARCMSR_HBCMU_OUTBOUND_POSTQUEUE_ISR_MASK);
		writel(intmask_org & mask, &reg->host_int_mask);
		acb->outbound_int_enable = ~(intmask_org & mask) & 0x0000000f;
		}
		break;
	case ACB_ADAPTER_TYPE_D: {
		struct MessageUnit_D *reg = acb->pmuD;

		mask = ARCMSR_ARC1214_ALL_INT_ENABLE;
		writel(intmask_org | mask, reg->pcief0_int_enable);
		break;
		}
	}
}
