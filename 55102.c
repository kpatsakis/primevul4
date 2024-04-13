void kvm_arch_irq_bypass_del_producer(struct irq_bypass_consumer *cons,
				      struct irq_bypass_producer *prod)
{
	struct kvm_kernel_irqfd *irqfd =
		container_of(cons, struct kvm_kernel_irqfd, consumer);
	struct kvm *kvm = irqfd->kvm;

	if (kvm->arch.kvm_ops->irq_bypass_del_producer)
		kvm->arch.kvm_ops->irq_bypass_del_producer(cons, prod);
}
