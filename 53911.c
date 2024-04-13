void kvm_arch_irq_bypass_del_producer(struct irq_bypass_consumer *cons,
				      struct irq_bypass_producer *prod)
{
	int ret;
	struct kvm_kernel_irqfd *irqfd =
		container_of(cons, struct kvm_kernel_irqfd, consumer);

	if (!kvm_x86_ops->update_pi_irte) {
		WARN_ON(irqfd->producer != NULL);
		return;
	}

	WARN_ON(irqfd->producer != prod);
	irqfd->producer = NULL;

	/*
	 * When producer of consumer is unregistered, we change back to
	 * remapped mode, so we can re-use the current implementation
	 * when the irq is masked/disabed or the consumer side (KVM
	 * int this case doesn't want to receive the interrupts.
	*/
	ret = kvm_x86_ops->update_pi_irte(irqfd->kvm, prod->irq, irqfd->gsi, 0);
	if (ret)
		printk(KERN_INFO "irq bypass consumer (token %p) unregistration"
		       " fails: %d\n", irqfd->consumer.token, ret);
}
