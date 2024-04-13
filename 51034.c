static void arcmsr_done4abort_postqueue(struct AdapterControlBlock *acb)
{
	int i = 0;
	uint32_t flag_ccb, ccb_cdb_phy;
	struct ARCMSR_CDB *pARCMSR_CDB;
	bool error;
	struct CommandControlBlock *pCCB;
	switch (acb->adapter_type) {

	case ACB_ADAPTER_TYPE_A: {
		struct MessageUnit_A __iomem *reg = acb->pmuA;
		uint32_t outbound_intstatus;
		outbound_intstatus = readl(&reg->outbound_intstatus) &
					acb->outbound_int_enable;
		/*clear and abort all outbound posted Q*/
		writel(outbound_intstatus, &reg->outbound_intstatus);/*clear interrupt*/
		while(((flag_ccb = readl(&reg->outbound_queueport)) != 0xFFFFFFFF)
				&& (i++ < ARCMSR_MAX_OUTSTANDING_CMD)) {
			pARCMSR_CDB = (struct ARCMSR_CDB *)(acb->vir2phy_offset + (flag_ccb << 5));/*frame must be 32 bytes aligned*/
			pCCB = container_of(pARCMSR_CDB, struct CommandControlBlock, arcmsr_cdb);
			error = (flag_ccb & ARCMSR_CCBREPLY_FLAG_ERROR_MODE0) ? true : false;
			arcmsr_drain_donequeue(acb, pCCB, error);
		}
		}
		break;

	case ACB_ADAPTER_TYPE_B: {
		struct MessageUnit_B *reg = acb->pmuB;
		/*clear all outbound posted Q*/
		writel(ARCMSR_DOORBELL_INT_CLEAR_PATTERN, reg->iop2drv_doorbell); /* clear doorbell interrupt */
		for (i = 0; i < ARCMSR_MAX_HBB_POSTQUEUE; i++) {
			flag_ccb = reg->done_qbuffer[i];
			if (flag_ccb != 0) {
				reg->done_qbuffer[i] = 0;
				pARCMSR_CDB = (struct ARCMSR_CDB *)(acb->vir2phy_offset+(flag_ccb << 5));/*frame must be 32 bytes aligned*/
				pCCB = container_of(pARCMSR_CDB, struct CommandControlBlock, arcmsr_cdb);
				error = (flag_ccb & ARCMSR_CCBREPLY_FLAG_ERROR_MODE0) ? true : false;
				arcmsr_drain_donequeue(acb, pCCB, error);
			}
			reg->post_qbuffer[i] = 0;
		}
		reg->doneq_index = 0;
		reg->postq_index = 0;
		}
		break;
	case ACB_ADAPTER_TYPE_C: {
		struct MessageUnit_C __iomem *reg = acb->pmuC;
		while ((readl(&reg->host_int_status) & ARCMSR_HBCMU_OUTBOUND_POSTQUEUE_ISR) && (i++ < ARCMSR_MAX_OUTSTANDING_CMD)) {
			/*need to do*/
			flag_ccb = readl(&reg->outbound_queueport_low);
			ccb_cdb_phy = (flag_ccb & 0xFFFFFFF0);
			pARCMSR_CDB = (struct  ARCMSR_CDB *)(acb->vir2phy_offset+ccb_cdb_phy);/*frame must be 32 bytes aligned*/
			pCCB = container_of(pARCMSR_CDB, struct CommandControlBlock, arcmsr_cdb);
			error = (flag_ccb & ARCMSR_CCBREPLY_FLAG_ERROR_MODE1) ? true : false;
			arcmsr_drain_donequeue(acb, pCCB, error);
		}
		}
		break;
	case ACB_ADAPTER_TYPE_D: {
		struct MessageUnit_D  *pmu = acb->pmuD;
		uint32_t outbound_write_pointer;
		uint32_t doneq_index, index_stripped, addressLow, residual, toggle;
		unsigned long flags;

		residual = atomic_read(&acb->ccboutstandingcount);
		for (i = 0; i < residual; i++) {
			spin_lock_irqsave(&acb->doneq_lock, flags);
			outbound_write_pointer =
				pmu->done_qbuffer[0].addressLow + 1;
			doneq_index = pmu->doneq_index;
			if ((doneq_index & 0xFFF) !=
				(outbound_write_pointer & 0xFFF)) {
				toggle = doneq_index & 0x4000;
				index_stripped = (doneq_index & 0xFFF) + 1;
				index_stripped %= ARCMSR_MAX_ARC1214_DONEQUEUE;
				pmu->doneq_index = index_stripped ? (index_stripped | toggle) :
					((toggle ^ 0x4000) + 1);
				doneq_index = pmu->doneq_index;
				spin_unlock_irqrestore(&acb->doneq_lock, flags);
				addressLow = pmu->done_qbuffer[doneq_index &
					0xFFF].addressLow;
				ccb_cdb_phy = (addressLow & 0xFFFFFFF0);
				pARCMSR_CDB = (struct  ARCMSR_CDB *)
					(acb->vir2phy_offset + ccb_cdb_phy);
				pCCB = container_of(pARCMSR_CDB,
					struct CommandControlBlock, arcmsr_cdb);
				error = (addressLow &
					ARCMSR_CCBREPLY_FLAG_ERROR_MODE1) ?
					true : false;
				arcmsr_drain_donequeue(acb, pCCB, error);
				writel(doneq_index,
					pmu->outboundlist_read_pointer);
			} else {
				spin_unlock_irqrestore(&acb->doneq_lock, flags);
				mdelay(10);
			}
		}
		pmu->postq_index = 0;
		pmu->doneq_index = 0x40FF;
		}
		break;
	}
}
