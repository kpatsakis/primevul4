static void arcmsr_free_ccb_pool(struct AdapterControlBlock *acb)
{
	dma_free_coherent(&acb->pdev->dev, acb->uncache_size, acb->dma_coherent, acb->dma_coherent_handle);
}
