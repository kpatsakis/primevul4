void kvm_irqfd_exit(void)
{
	destroy_workqueue(irqfd_cleanup_wq);
}
