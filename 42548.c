static void kvm_free_assigned_device(struct kvm *kvm,
				     struct kvm_assigned_dev_kernel
				     *assigned_dev)
{
	kvm_free_assigned_irq(kvm, assigned_dev);

	pci_reset_function(assigned_dev->dev);
	if (pci_load_and_free_saved_state(assigned_dev->dev,
					  &assigned_dev->pci_saved_state))
		printk(KERN_INFO "%s: Couldn't reload %s saved state\n",
		       __func__, dev_name(&assigned_dev->dev->dev));
	else
		pci_restore_state(assigned_dev->dev);

	pci_release_regions(assigned_dev->dev);
	pci_disable_device(assigned_dev->dev);
	pci_dev_put(assigned_dev->dev);

	list_del(&assigned_dev->list);
	kfree(assigned_dev);
}
