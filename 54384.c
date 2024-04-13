int __attribute__((weak)) kvm_arch_set_irq_inatomic(
				struct kvm_kernel_irq_routing_entry *irq,
				struct kvm *kvm, int irq_source_id,
				int level,
				bool line_status)
{
	return -EWOULDBLOCK;
}
