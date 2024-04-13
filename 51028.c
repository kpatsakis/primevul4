static int arcmsr_build_ccb(struct AdapterControlBlock *acb,
	struct CommandControlBlock *ccb, struct scsi_cmnd *pcmd)
{
	struct ARCMSR_CDB *arcmsr_cdb = (struct ARCMSR_CDB *)&ccb->arcmsr_cdb;
	int8_t *psge = (int8_t *)&arcmsr_cdb->u;
	__le32 address_lo, address_hi;
	int arccdbsize = 0x30;
	__le32 length = 0;
	int i;
	struct scatterlist *sg;
	int nseg;
	ccb->pcmd = pcmd;
	memset(arcmsr_cdb, 0, sizeof(struct ARCMSR_CDB));
	arcmsr_cdb->TargetID = pcmd->device->id;
	arcmsr_cdb->LUN = pcmd->device->lun;
	arcmsr_cdb->Function = 1;
	arcmsr_cdb->msgContext = 0;
	memcpy(arcmsr_cdb->Cdb, pcmd->cmnd, pcmd->cmd_len);

	nseg = scsi_dma_map(pcmd);
	if (unlikely(nseg > acb->host->sg_tablesize || nseg < 0))
		return FAILED;
	scsi_for_each_sg(pcmd, sg, nseg, i) {
		/* Get the physical address of the current data pointer */
		length = cpu_to_le32(sg_dma_len(sg));
		address_lo = cpu_to_le32(dma_addr_lo32(sg_dma_address(sg)));
		address_hi = cpu_to_le32(dma_addr_hi32(sg_dma_address(sg)));
		if (address_hi == 0) {
			struct SG32ENTRY *pdma_sg = (struct SG32ENTRY *)psge;

			pdma_sg->address = address_lo;
			pdma_sg->length = length;
			psge += sizeof (struct SG32ENTRY);
			arccdbsize += sizeof (struct SG32ENTRY);
		} else {
			struct SG64ENTRY *pdma_sg = (struct SG64ENTRY *)psge;

			pdma_sg->addresshigh = address_hi;
			pdma_sg->address = address_lo;
			pdma_sg->length = length|cpu_to_le32(IS_SG64_ADDR);
			psge += sizeof (struct SG64ENTRY);
			arccdbsize += sizeof (struct SG64ENTRY);
		}
	}
	arcmsr_cdb->sgcount = (uint8_t)nseg;
	arcmsr_cdb->DataLength = scsi_bufflen(pcmd);
	arcmsr_cdb->msgPages = arccdbsize/0x100 + (arccdbsize % 0x100 ? 1 : 0);
	if ( arccdbsize > 256)
		arcmsr_cdb->Flags |= ARCMSR_CDB_FLAG_SGL_BSIZE;
	if (pcmd->sc_data_direction == DMA_TO_DEVICE)
		arcmsr_cdb->Flags |= ARCMSR_CDB_FLAG_WRITE;
	ccb->arc_cdb_size = arccdbsize;
	return SUCCESS;
}
