static int arcmsr_iop_confirm(struct AdapterControlBlock *acb)
{
	uint32_t cdb_phyaddr, cdb_phyaddr_hi32;
	dma_addr_t dma_coherent_handle;

	/*
	********************************************************************
	** here we need to tell iop 331 our freeccb.HighPart
	** if freeccb.HighPart is not zero
	********************************************************************
	*/
	switch (acb->adapter_type) {
	case ACB_ADAPTER_TYPE_B:
	case ACB_ADAPTER_TYPE_D:
		dma_coherent_handle = acb->dma_coherent_handle2;
		break;
	default:
		dma_coherent_handle = acb->dma_coherent_handle;
		break;
	}
	cdb_phyaddr = lower_32_bits(dma_coherent_handle);
	cdb_phyaddr_hi32 = upper_32_bits(dma_coherent_handle);
	acb->cdb_phyaddr_hi32 = cdb_phyaddr_hi32;
	/*
	***********************************************************************
	**    if adapter type B, set window of "post command Q"
	***********************************************************************
	*/
	switch (acb->adapter_type) {

	case ACB_ADAPTER_TYPE_A: {
		if (cdb_phyaddr_hi32 != 0) {
			struct MessageUnit_A __iomem *reg = acb->pmuA;
			writel(ARCMSR_SIGNATURE_SET_CONFIG, \
						&reg->message_rwbuffer[0]);
			writel(cdb_phyaddr_hi32, &reg->message_rwbuffer[1]);
			writel(ARCMSR_INBOUND_MESG0_SET_CONFIG, \
							&reg->inbound_msgaddr0);
			if (!arcmsr_hbaA_wait_msgint_ready(acb)) {
				printk(KERN_NOTICE "arcmsr%d: ""set ccb high \
				part physical address timeout\n",
				acb->host->host_no);
				return 1;
			}
		}
		}
		break;

	case ACB_ADAPTER_TYPE_B: {
		uint32_t __iomem *rwbuffer;

		struct MessageUnit_B *reg = acb->pmuB;
		reg->postq_index = 0;
		reg->doneq_index = 0;
		writel(ARCMSR_MESSAGE_SET_POST_WINDOW, reg->drv2iop_doorbell);
		if (!arcmsr_hbaB_wait_msgint_ready(acb)) {
			printk(KERN_NOTICE "arcmsr%d: cannot set driver mode\n", \
				acb->host->host_no);
			return 1;
		}
		rwbuffer = reg->message_rwbuffer;
		/* driver "set config" signature */
		writel(ARCMSR_SIGNATURE_SET_CONFIG, rwbuffer++);
		/* normal should be zero */
		writel(cdb_phyaddr_hi32, rwbuffer++);
		/* postQ size (256 + 8)*4	 */
		writel(cdb_phyaddr, rwbuffer++);
		/* doneQ size (256 + 8)*4	 */
		writel(cdb_phyaddr + 1056, rwbuffer++);
		/* ccb maxQ size must be --> [(256 + 8)*4]*/
		writel(1056, rwbuffer);

		writel(ARCMSR_MESSAGE_SET_CONFIG, reg->drv2iop_doorbell);
		if (!arcmsr_hbaB_wait_msgint_ready(acb)) {
			printk(KERN_NOTICE "arcmsr%d: 'set command Q window' \
			timeout \n",acb->host->host_no);
			return 1;
		}
		writel(ARCMSR_MESSAGE_START_DRIVER_MODE, reg->drv2iop_doorbell);
		if (!arcmsr_hbaB_wait_msgint_ready(acb)) {
			pr_err("arcmsr%d: can't set driver mode.\n",
				acb->host->host_no);
			return 1;
		}
		}
		break;
	case ACB_ADAPTER_TYPE_C: {
		if (cdb_phyaddr_hi32 != 0) {
			struct MessageUnit_C __iomem *reg = acb->pmuC;

			printk(KERN_NOTICE "arcmsr%d: cdb_phyaddr_hi32=0x%x\n",
					acb->adapter_index, cdb_phyaddr_hi32);
			writel(ARCMSR_SIGNATURE_SET_CONFIG, &reg->msgcode_rwbuffer[0]);
			writel(cdb_phyaddr_hi32, &reg->msgcode_rwbuffer[1]);
			writel(ARCMSR_INBOUND_MESG0_SET_CONFIG, &reg->inbound_msgaddr0);
			writel(ARCMSR_HBCMU_DRV2IOP_MESSAGE_CMD_DONE, &reg->inbound_doorbell);
			if (!arcmsr_hbaC_wait_msgint_ready(acb)) {
				printk(KERN_NOTICE "arcmsr%d: 'set command Q window' \
				timeout \n", acb->host->host_no);
				return 1;
			}
		}
		}
		break;
	case ACB_ADAPTER_TYPE_D: {
		uint32_t __iomem *rwbuffer;
		struct MessageUnit_D *reg = acb->pmuD;
		reg->postq_index = 0;
		reg->doneq_index = 0;
		rwbuffer = reg->msgcode_rwbuffer;
		writel(ARCMSR_SIGNATURE_SET_CONFIG, rwbuffer++);
		writel(cdb_phyaddr_hi32, rwbuffer++);
		writel(cdb_phyaddr, rwbuffer++);
		writel(cdb_phyaddr + (ARCMSR_MAX_ARC1214_POSTQUEUE *
			sizeof(struct InBound_SRB)), rwbuffer++);
		writel(0x100, rwbuffer);
		writel(ARCMSR_INBOUND_MESG0_SET_CONFIG, reg->inbound_msgaddr0);
		if (!arcmsr_hbaD_wait_msgint_ready(acb)) {
			pr_notice("arcmsr%d: 'set command Q window' timeout\n",
				acb->host->host_no);
			return 1;
		}
		}
		break;
	}
	return 0;
}
