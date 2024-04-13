int kvm_arch_irq_bypass_add_producer(struct irq_bypass_consumer *cons,
				     struct irq_bypass_producer *prod)
{
	struct kvm_kernel_irqfd *irqfd =
		container_of(cons, struct kvm_kernel_irqfd, consumer);
	struct kvm *kvm = irqfd->kvm;

	if (kvm->arch.kvm_ops->irq_bypass_add_producer)
		return kvm->arch.kvm_ops->irq_bypass_add_producer(cons, prod);

	return 0;
}
