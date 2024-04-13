static bool arcmsr_alloc_io_queue(struct AdapterControlBlock *acb)
{
	bool rtn = true;
	void *dma_coherent;
	dma_addr_t dma_coherent_handle;
	struct pci_dev *pdev = acb->pdev;

	switch (acb->adapter_type) {
	case ACB_ADAPTER_TYPE_B: {
		struct MessageUnit_B *reg;
		acb->roundup_ccbsize = roundup(sizeof(struct MessageUnit_B), 32);
		dma_coherent = dma_zalloc_coherent(&pdev->dev, acb->roundup_ccbsize,
			&dma_coherent_handle, GFP_KERNEL);
		if (!dma_coherent) {
			pr_notice("arcmsr%d: DMA allocation failed\n", acb->host->host_no);
			return false;
		}
		acb->dma_coherent_handle2 = dma_coherent_handle;
		acb->dma_coherent2 = dma_coherent;
		reg = (struct MessageUnit_B *)dma_coherent;
		acb->pmuB = reg;
		if (acb->pdev->device == PCI_DEVICE_ID_ARECA_1203) {
			reg->drv2iop_doorbell = MEM_BASE0(ARCMSR_DRV2IOP_DOORBELL_1203);
			reg->drv2iop_doorbell_mask = MEM_BASE0(ARCMSR_DRV2IOP_DOORBELL_MASK_1203);
			reg->iop2drv_doorbell = MEM_BASE0(ARCMSR_IOP2DRV_DOORBELL_1203);
			reg->iop2drv_doorbell_mask = MEM_BASE0(ARCMSR_IOP2DRV_DOORBELL_MASK_1203);
		} else {
			reg->drv2iop_doorbell = MEM_BASE0(ARCMSR_DRV2IOP_DOORBELL);
			reg->drv2iop_doorbell_mask = MEM_BASE0(ARCMSR_DRV2IOP_DOORBELL_MASK);
			reg->iop2drv_doorbell = MEM_BASE0(ARCMSR_IOP2DRV_DOORBELL);
			reg->iop2drv_doorbell_mask = MEM_BASE0(ARCMSR_IOP2DRV_DOORBELL_MASK);
		}
		reg->message_wbuffer = MEM_BASE1(ARCMSR_MESSAGE_WBUFFER);
		reg->message_rbuffer = MEM_BASE1(ARCMSR_MESSAGE_RBUFFER);
		reg->message_rwbuffer = MEM_BASE1(ARCMSR_MESSAGE_RWBUFFER);
		}
		break;
	case ACB_ADAPTER_TYPE_D: {
		struct MessageUnit_D *reg;

		acb->roundup_ccbsize = roundup(sizeof(struct MessageUnit_D), 32);
		dma_coherent = dma_zalloc_coherent(&pdev->dev, acb->roundup_ccbsize,
			&dma_coherent_handle, GFP_KERNEL);
		if (!dma_coherent) {
			pr_notice("arcmsr%d: DMA allocation failed\n", acb->host->host_no);
			return false;
		}
		acb->dma_coherent_handle2 = dma_coherent_handle;
		acb->dma_coherent2 = dma_coherent;
		reg = (struct MessageUnit_D *)dma_coherent;
		acb->pmuD = reg;
		reg->chip_id = MEM_BASE0(ARCMSR_ARC1214_CHIP_ID);
		reg->cpu_mem_config = MEM_BASE0(ARCMSR_ARC1214_CPU_MEMORY_CONFIGURATION);
		reg->i2o_host_interrupt_mask = MEM_BASE0(ARCMSR_ARC1214_I2_HOST_INTERRUPT_MASK);
		reg->sample_at_reset = MEM_BASE0(ARCMSR_ARC1214_SAMPLE_RESET);
		reg->reset_request = MEM_BASE0(ARCMSR_ARC1214_RESET_REQUEST);
		reg->host_int_status = MEM_BASE0(ARCMSR_ARC1214_MAIN_INTERRUPT_STATUS);
		reg->pcief0_int_enable = MEM_BASE0(ARCMSR_ARC1214_PCIE_F0_INTERRUPT_ENABLE);
		reg->inbound_msgaddr0 = MEM_BASE0(ARCMSR_ARC1214_INBOUND_MESSAGE0);
		reg->inbound_msgaddr1 = MEM_BASE0(ARCMSR_ARC1214_INBOUND_MESSAGE1);
		reg->outbound_msgaddr0 = MEM_BASE0(ARCMSR_ARC1214_OUTBOUND_MESSAGE0);
		reg->outbound_msgaddr1 = MEM_BASE0(ARCMSR_ARC1214_OUTBOUND_MESSAGE1);
		reg->inbound_doorbell = MEM_BASE0(ARCMSR_ARC1214_INBOUND_DOORBELL);
		reg->outbound_doorbell = MEM_BASE0(ARCMSR_ARC1214_OUTBOUND_DOORBELL);
		reg->outbound_doorbell_enable = MEM_BASE0(ARCMSR_ARC1214_OUTBOUND_DOORBELL_ENABLE);
		reg->inboundlist_base_low = MEM_BASE0(ARCMSR_ARC1214_INBOUND_LIST_BASE_LOW);
		reg->inboundlist_base_high = MEM_BASE0(ARCMSR_ARC1214_INBOUND_LIST_BASE_HIGH);
		reg->inboundlist_write_pointer = MEM_BASE0(ARCMSR_ARC1214_INBOUND_LIST_WRITE_POINTER);
		reg->outboundlist_base_low = MEM_BASE0(ARCMSR_ARC1214_OUTBOUND_LIST_BASE_LOW);
		reg->outboundlist_base_high = MEM_BASE0(ARCMSR_ARC1214_OUTBOUND_LIST_BASE_HIGH);
		reg->outboundlist_copy_pointer = MEM_BASE0(ARCMSR_ARC1214_OUTBOUND_LIST_COPY_POINTER);
		reg->outboundlist_read_pointer = MEM_BASE0(ARCMSR_ARC1214_OUTBOUND_LIST_READ_POINTER);
		reg->outboundlist_interrupt_cause = MEM_BASE0(ARCMSR_ARC1214_OUTBOUND_INTERRUPT_CAUSE);
		reg->outboundlist_interrupt_enable = MEM_BASE0(ARCMSR_ARC1214_OUTBOUND_INTERRUPT_ENABLE);
		reg->message_wbuffer = MEM_BASE0(ARCMSR_ARC1214_MESSAGE_WBUFFER);
		reg->message_rbuffer = MEM_BASE0(ARCMSR_ARC1214_MESSAGE_RBUFFER);
		reg->msgcode_rwbuffer = MEM_BASE0(ARCMSR_ARC1214_MESSAGE_RWBUFFER);
		}
		break;
	default:
		break;
	}
	return rtn;
}
