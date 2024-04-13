irqfd_is_active(struct kvm_kernel_irqfd *irqfd)
{
	return list_empty(&irqfd->list) ? false : true;
}
