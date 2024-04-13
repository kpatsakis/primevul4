irqfd_resampler_ack(struct kvm_irq_ack_notifier *kian)
{
	struct kvm_kernel_irqfd_resampler *resampler;
	struct kvm *kvm;
	struct kvm_kernel_irqfd *irqfd;
	int idx;

	resampler = container_of(kian,
			struct kvm_kernel_irqfd_resampler, notifier);
	kvm = resampler->kvm;

	kvm_set_irq(kvm, KVM_IRQFD_RESAMPLE_IRQ_SOURCE_ID,
		    resampler->notifier.gsi, 0, false);

	idx = srcu_read_lock(&kvm->irq_srcu);

	list_for_each_entry_rcu(irqfd, &resampler->list, resampler_link)
		eventfd_signal(irqfd->resamplefd, 1);

	srcu_read_unlock(&kvm->irq_srcu, idx);
}
