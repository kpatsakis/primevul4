irqfd_wakeup(wait_queue_entry_t *wait, unsigned mode, int sync, void *key)
{
	struct kvm_kernel_irqfd *irqfd =
		container_of(wait, struct kvm_kernel_irqfd, wait);
	unsigned long flags = (unsigned long)key;
	struct kvm_kernel_irq_routing_entry irq;
	struct kvm *kvm = irqfd->kvm;
	unsigned seq;
	int idx;

	if (flags & POLLIN) {
		idx = srcu_read_lock(&kvm->irq_srcu);
		do {
			seq = read_seqcount_begin(&irqfd->irq_entry_sc);
			irq = irqfd->irq_entry;
		} while (read_seqcount_retry(&irqfd->irq_entry_sc, seq));
		/* An event has been signaled, inject an interrupt */
		if (kvm_arch_set_irq_inatomic(&irq, kvm,
					      KVM_USERSPACE_IRQ_SOURCE_ID, 1,
					      false) == -EWOULDBLOCK)
			schedule_work(&irqfd->inject);
		srcu_read_unlock(&kvm->irq_srcu, idx);
	}

	if (flags & POLLHUP) {
		/* The eventfd is closing, detach from KVM */
		unsigned long flags;

		spin_lock_irqsave(&kvm->irqfds.lock, flags);

		/*
		 * We must check if someone deactivated the irqfd before
		 * we could acquire the irqfds.lock since the item is
		 * deactivated from the KVM side before it is unhooked from
		 * the wait-queue.  If it is already deactivated, we can
		 * simply return knowing the other side will cleanup for us.
		 * We cannot race against the irqfd going away since the
		 * other side is required to acquire wqh->lock, which we hold
		 */
		if (irqfd_is_active(irqfd))
			irqfd_deactivate(irqfd);

		spin_unlock_irqrestore(&kvm->irqfds.lock, flags);
	}

	return 0;
}
