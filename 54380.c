irqfd_shutdown(struct work_struct *work)
{
	struct kvm_kernel_irqfd *irqfd =
		container_of(work, struct kvm_kernel_irqfd, shutdown);
	u64 cnt;

	/*
	 * Synchronize with the wait-queue and unhook ourselves to prevent
	 * further events.
	 */
	eventfd_ctx_remove_wait_queue(irqfd->eventfd, &irqfd->wait, &cnt);

	/*
	 * We know no new events will be scheduled at this point, so block
	 * until all previously outstanding events have completed
	 */
	flush_work(&irqfd->inject);

	if (irqfd->resampler) {
		irqfd_resampler_shutdown(irqfd);
		eventfd_ctx_put(irqfd->resamplefd);
	}

	/*
	 * It is now safe to release the object's resources
	 */
#ifdef CONFIG_HAVE_KVM_IRQ_BYPASS
	irq_bypass_unregister_consumer(&irqfd->consumer);
#endif
	eventfd_ctx_put(irqfd->eventfd);
	kfree(irqfd);
}
