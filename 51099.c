static void arcmsr_pci_unmap_dma(struct CommandControlBlock *ccb)
{
	struct scsi_cmnd *pcmd = ccb->pcmd;

	scsi_dma_unmap(pcmd);
}
