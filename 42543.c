static int find_index_from_host_irq(struct kvm_assigned_dev_kernel
				    *assigned_dev, int irq)
{
	int i, index;
	struct msix_entry *host_msix_entries;

	host_msix_entries = assigned_dev->host_msix_entries;

	index = -1;
	for (i = 0; i < assigned_dev->entries_nr; i++)
		if (irq == host_msix_entries[i].vector) {
			index = i;
			break;
		}
	if (index < 0) {
		printk(KERN_WARNING "Fail to find correlated MSI-X entry!\n");
		return 0;
	}

	return index;
}
