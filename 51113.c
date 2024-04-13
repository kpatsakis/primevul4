static void arcmsr_wait_firmware_ready(struct AdapterControlBlock *acb)
{
	uint32_t firmware_state = 0;
	switch (acb->adapter_type) {

	case ACB_ADAPTER_TYPE_A: {
		struct MessageUnit_A __iomem *reg = acb->pmuA;
		do {
			firmware_state = readl(&reg->outbound_msgaddr1);
		} while ((firmware_state & ARCMSR_OUTBOUND_MESG1_FIRMWARE_OK) == 0);
		}
		break;

	case ACB_ADAPTER_TYPE_B: {
		struct MessageUnit_B *reg = acb->pmuB;
		do {
			firmware_state = readl(reg->iop2drv_doorbell);
		} while ((firmware_state & ARCMSR_MESSAGE_FIRMWARE_OK) == 0);
		writel(ARCMSR_DRV2IOP_END_OF_INTERRUPT, reg->drv2iop_doorbell);
		}
		break;
	case ACB_ADAPTER_TYPE_C: {
		struct MessageUnit_C __iomem *reg = acb->pmuC;
		do {
			firmware_state = readl(&reg->outbound_msgaddr1);
		} while ((firmware_state & ARCMSR_HBCMU_MESSAGE_FIRMWARE_OK) == 0);
		}
		break;
	case ACB_ADAPTER_TYPE_D: {
		struct MessageUnit_D *reg = acb->pmuD;
		do {
			firmware_state = readl(reg->outbound_msgaddr1);
		} while ((firmware_state &
			ARCMSR_ARC1214_MESSAGE_FIRMWARE_OK) == 0);
		}
		break;
	}
}
