int kvm_irqfd_init(void)
{
	irqfd_cleanup_wq = alloc_workqueue("kvm-irqfd-cleanup", 0, 0);
	if (!irqfd_cleanup_wq)
		return -ENOMEM;

	return 0;
}
