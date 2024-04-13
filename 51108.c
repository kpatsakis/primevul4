arcmsr_request_irq(struct pci_dev *pdev, struct AdapterControlBlock *acb)
{
	int	i, j, r;
	struct msix_entry entries[ARCMST_NUM_MSIX_VECTORS];

	for (i = 0; i < ARCMST_NUM_MSIX_VECTORS; i++)
		entries[i].entry = i;
	r = pci_enable_msix_range(pdev, entries, 1, ARCMST_NUM_MSIX_VECTORS);
	if (r < 0)
		goto msi_int;
	acb->msix_vector_count = r;
	for (i = 0; i < r; i++) {
		if (request_irq(entries[i].vector,
			arcmsr_do_interrupt, 0, "arcmsr", acb)) {
			pr_warn("arcmsr%d: request_irq =%d failed!\n",
				acb->host->host_no, entries[i].vector);
			for (j = 0 ; j < i ; j++)
				free_irq(entries[j].vector, acb);
			pci_disable_msix(pdev);
			goto msi_int;
		}
		acb->entries[i] = entries[i];
	}
	acb->acb_flags |= ACB_F_MSIX_ENABLED;
	pr_info("arcmsr%d: msi-x enabled\n", acb->host->host_no);
	return SUCCESS;
msi_int:
	if (pci_enable_msi_exact(pdev, 1) < 0)
		goto legacy_int;
	if (request_irq(pdev->irq, arcmsr_do_interrupt,
		IRQF_SHARED, "arcmsr", acb)) {
		pr_warn("arcmsr%d: request_irq =%d failed!\n",
			acb->host->host_no, pdev->irq);
		pci_disable_msi(pdev);
		goto legacy_int;
	}
	acb->acb_flags |= ACB_F_MSI_ENABLED;
	pr_info("arcmsr%d: msi enabled\n", acb->host->host_no);
	return SUCCESS;
legacy_int:
	if (request_irq(pdev->irq, arcmsr_do_interrupt,
		IRQF_SHARED, "arcmsr", acb)) {
		pr_warn("arcmsr%d: request_irq = %d failed!\n",
			acb->host->host_no, pdev->irq);
		return FAILED;
	}
	return SUCCESS;
}
