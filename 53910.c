int kvm_arch_irq_bypass_add_producer(struct irq_bypass_consumer *cons,
				      struct irq_bypass_producer *prod)
{
	struct kvm_kernel_irqfd *irqfd =
		container_of(cons, struct kvm_kernel_irqfd, consumer);

	if (kvm_x86_ops->update_pi_irte) {
		irqfd->producer = prod;
		return kvm_x86_ops->update_pi_irte(irqfd->kvm,
				prod->irq, irqfd->gsi, 1);
	}

	return -EINVAL;
}
