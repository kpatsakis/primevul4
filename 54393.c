kvm_irqfd_deassign(struct kvm *kvm, struct kvm_irqfd *args)
{
	struct kvm_kernel_irqfd *irqfd, *tmp;
	struct eventfd_ctx *eventfd;

	eventfd = eventfd_ctx_fdget(args->fd);
	if (IS_ERR(eventfd))
		return PTR_ERR(eventfd);

	spin_lock_irq(&kvm->irqfds.lock);

	list_for_each_entry_safe(irqfd, tmp, &kvm->irqfds.items, list) {
		if (irqfd->eventfd == eventfd && irqfd->gsi == args->gsi) {
			/*
			 * This clearing of irq_entry.type is needed for when
			 * another thread calls kvm_irq_routing_update before
			 * we flush workqueue below (we synchronize with
			 * kvm_irq_routing_update using irqfds.lock).
			 */
			write_seqcount_begin(&irqfd->irq_entry_sc);
			irqfd->irq_entry.type = 0;
			write_seqcount_end(&irqfd->irq_entry_sc);
			irqfd_deactivate(irqfd);
		}
	}

	spin_unlock_irq(&kvm->irqfds.lock);
	eventfd_ctx_put(eventfd);

	/*
	 * Block until we know all outstanding shutdown jobs have completed
	 * so that we guarantee there will not be any more interrupts on this
	 * gsi once this deassign function returns.
	 */
	flush_workqueue(irqfd_cleanup_wq);

	return 0;
}
