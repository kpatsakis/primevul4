static void arcmsr_free_irq(struct pci_dev *pdev,
		struct AdapterControlBlock *acb)
{
	int i;

	if (acb->acb_flags & ACB_F_MSI_ENABLED) {
		free_irq(pdev->irq, acb);
		pci_disable_msi(pdev);
	} else if (acb->acb_flags & ACB_F_MSIX_ENABLED) {
		for (i = 0; i < acb->msix_vector_count; i++)
			free_irq(acb->entries[i].vector, acb);
		pci_disable_msix(pdev);
	} else
		free_irq(pdev->irq, acb);
}
