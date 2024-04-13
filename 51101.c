static void arcmsr_post_ccb(struct AdapterControlBlock *acb, struct CommandControlBlock *ccb)
{
	uint32_t cdb_phyaddr = ccb->cdb_phyaddr;
	struct ARCMSR_CDB *arcmsr_cdb = (struct ARCMSR_CDB *)&ccb->arcmsr_cdb;
	atomic_inc(&acb->ccboutstandingcount);
	ccb->startdone = ARCMSR_CCB_START;
	switch (acb->adapter_type) {
	case ACB_ADAPTER_TYPE_A: {
		struct MessageUnit_A __iomem *reg = acb->pmuA;

		if (arcmsr_cdb->Flags & ARCMSR_CDB_FLAG_SGL_BSIZE)
			writel(cdb_phyaddr | ARCMSR_CCBPOST_FLAG_SGL_BSIZE,
			&reg->inbound_queueport);
		else
			writel(cdb_phyaddr, &reg->inbound_queueport);
		break;
	}

	case ACB_ADAPTER_TYPE_B: {
		struct MessageUnit_B *reg = acb->pmuB;
		uint32_t ending_index, index = reg->postq_index;

		ending_index = ((index + 1) % ARCMSR_MAX_HBB_POSTQUEUE);
		reg->post_qbuffer[ending_index] = 0;
		if (arcmsr_cdb->Flags & ARCMSR_CDB_FLAG_SGL_BSIZE) {
			reg->post_qbuffer[index] =
				cdb_phyaddr | ARCMSR_CCBPOST_FLAG_SGL_BSIZE;
		} else {
			reg->post_qbuffer[index] = cdb_phyaddr;
		}
		index++;
		index %= ARCMSR_MAX_HBB_POSTQUEUE;/*if last index number set it to 0 */
		reg->postq_index = index;
		writel(ARCMSR_DRV2IOP_CDB_POSTED, reg->drv2iop_doorbell);
		}
		break;
	case ACB_ADAPTER_TYPE_C: {
		struct MessageUnit_C __iomem *phbcmu = acb->pmuC;
		uint32_t ccb_post_stamp, arc_cdb_size;

		arc_cdb_size = (ccb->arc_cdb_size > 0x300) ? 0x300 : ccb->arc_cdb_size;
		ccb_post_stamp = (cdb_phyaddr | ((arc_cdb_size - 1) >> 6) | 1);
		if (acb->cdb_phyaddr_hi32) {
			writel(acb->cdb_phyaddr_hi32, &phbcmu->inbound_queueport_high);
			writel(ccb_post_stamp, &phbcmu->inbound_queueport_low);
		} else {
			writel(ccb_post_stamp, &phbcmu->inbound_queueport_low);
		}
		}
		break;
	case ACB_ADAPTER_TYPE_D: {
		struct MessageUnit_D  *pmu = acb->pmuD;
		u16 index_stripped;
		u16 postq_index, toggle;
		unsigned long flags;
		struct InBound_SRB *pinbound_srb;

		spin_lock_irqsave(&acb->postq_lock, flags);
		postq_index = pmu->postq_index;
		pinbound_srb = (struct InBound_SRB *)&(pmu->post_qbuffer[postq_index & 0xFF]);
		pinbound_srb->addressHigh = dma_addr_hi32(cdb_phyaddr);
		pinbound_srb->addressLow = dma_addr_lo32(cdb_phyaddr);
		pinbound_srb->length = ccb->arc_cdb_size >> 2;
		arcmsr_cdb->msgContext = dma_addr_lo32(cdb_phyaddr);
		toggle = postq_index & 0x4000;
		index_stripped = postq_index + 1;
		index_stripped &= (ARCMSR_MAX_ARC1214_POSTQUEUE - 1);
		pmu->postq_index = index_stripped ? (index_stripped | toggle) :
			(toggle ^ 0x4000);
		writel(postq_index, pmu->inboundlist_write_pointer);
		spin_unlock_irqrestore(&acb->postq_lock, flags);
		break;
		}
	}
}
