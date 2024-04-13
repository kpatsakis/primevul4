static void arcmsr_free_mu(struct AdapterControlBlock *acb)
{
	switch (acb->adapter_type) {
	case ACB_ADAPTER_TYPE_B:
	case ACB_ADAPTER_TYPE_D: {
		dma_free_coherent(&acb->pdev->dev, acb->roundup_ccbsize,
			acb->dma_coherent2, acb->dma_coherent_handle2);
		break;
	}
	}
}
