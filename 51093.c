static void arcmsr_iop_message_wrote(struct AdapterControlBlock *acb)
{
	switch (acb->adapter_type) {
	case ACB_ADAPTER_TYPE_A: {
		struct MessageUnit_A __iomem *reg = acb->pmuA;
		/*
		** push inbound doorbell tell iop, driver data write ok
		** and wait reply on next hwinterrupt for next Qbuffer post
		*/
		writel(ARCMSR_INBOUND_DRIVER_DATA_WRITE_OK, &reg->inbound_doorbell);
		}
		break;

	case ACB_ADAPTER_TYPE_B: {
		struct MessageUnit_B *reg = acb->pmuB;
		/*
		** push inbound doorbell tell iop, driver data write ok
		** and wait reply on next hwinterrupt for next Qbuffer post
		*/
		writel(ARCMSR_DRV2IOP_DATA_WRITE_OK, reg->drv2iop_doorbell);
		}
		break;
	case ACB_ADAPTER_TYPE_C: {
		struct MessageUnit_C __iomem *reg = acb->pmuC;
		/*
		** push inbound doorbell tell iop, driver data write ok
		** and wait reply on next hwinterrupt for next Qbuffer post
		*/
		writel(ARCMSR_HBCMU_DRV2IOP_DATA_WRITE_OK, &reg->inbound_doorbell);
		}
		break;
	case ACB_ADAPTER_TYPE_D: {
		struct MessageUnit_D *reg = acb->pmuD;
		writel(ARCMSR_ARC1214_DRV2IOP_DATA_IN_READY,
			reg->inbound_doorbell);
		}
		break;
	}
}
