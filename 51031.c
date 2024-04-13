static void arcmsr_clear_doorbell_queue_buffer(struct AdapterControlBlock *acb)
{
	switch (acb->adapter_type) {
	case ACB_ADAPTER_TYPE_A: {
		struct MessageUnit_A __iomem *reg = acb->pmuA;
		uint32_t outbound_doorbell;
		/* empty doorbell Qbuffer if door bell ringed */
		outbound_doorbell = readl(&reg->outbound_doorbell);
		/*clear doorbell interrupt */
		writel(outbound_doorbell, &reg->outbound_doorbell);
		writel(ARCMSR_INBOUND_DRIVER_DATA_READ_OK, &reg->inbound_doorbell);
		}
		break;

	case ACB_ADAPTER_TYPE_B: {
		struct MessageUnit_B *reg = acb->pmuB;
		/*clear interrupt and message state*/
		writel(ARCMSR_MESSAGE_INT_CLEAR_PATTERN, reg->iop2drv_doorbell);
		writel(ARCMSR_DRV2IOP_DATA_READ_OK, reg->drv2iop_doorbell);
		/* let IOP know data has been read */
		}
		break;
	case ACB_ADAPTER_TYPE_C: {
		struct MessageUnit_C __iomem *reg = acb->pmuC;
		uint32_t outbound_doorbell, i;
		/* empty doorbell Qbuffer if door bell ringed */
		outbound_doorbell = readl(&reg->outbound_doorbell);
		writel(outbound_doorbell, &reg->outbound_doorbell_clear);
		writel(ARCMSR_HBCMU_DRV2IOP_DATA_READ_OK, &reg->inbound_doorbell);
		for (i = 0; i < 200; i++) {
			msleep(20);
			outbound_doorbell = readl(&reg->outbound_doorbell);
			if (outbound_doorbell &
				ARCMSR_HBCMU_IOP2DRV_DATA_WRITE_OK) {
				writel(outbound_doorbell,
					&reg->outbound_doorbell_clear);
				writel(ARCMSR_HBCMU_DRV2IOP_DATA_READ_OK,
					&reg->inbound_doorbell);
			} else
				break;
		}
		}
		break;
	case ACB_ADAPTER_TYPE_D: {
		struct MessageUnit_D *reg = acb->pmuD;
		uint32_t outbound_doorbell, i;
		/* empty doorbell Qbuffer if door bell ringed */
		outbound_doorbell = readl(reg->outbound_doorbell);
		writel(outbound_doorbell, reg->outbound_doorbell);
		writel(ARCMSR_ARC1214_DRV2IOP_DATA_OUT_READ,
			reg->inbound_doorbell);
		for (i = 0; i < 200; i++) {
			msleep(20);
			outbound_doorbell = readl(reg->outbound_doorbell);
			if (outbound_doorbell &
				ARCMSR_ARC1214_IOP2DRV_DATA_WRITE_OK) {
				writel(outbound_doorbell,
					reg->outbound_doorbell);
				writel(ARCMSR_ARC1214_DRV2IOP_DATA_OUT_READ,
					reg->inbound_doorbell);
			} else
				break;
		}
		}
		break;
	}
}
