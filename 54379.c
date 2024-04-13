irqfd_resampler_shutdown(struct kvm_kernel_irqfd *irqfd)
{
	struct kvm_kernel_irqfd_resampler *resampler = irqfd->resampler;
	struct kvm *kvm = resampler->kvm;

	mutex_lock(&kvm->irqfds.resampler_lock);

	list_del_rcu(&irqfd->resampler_link);
	synchronize_srcu(&kvm->irq_srcu);

	if (list_empty(&resampler->list)) {
		list_del(&resampler->link);
		kvm_unregister_irq_ack_notifier(kvm, &resampler->notifier);
		kvm_set_irq(kvm, KVM_IRQFD_RESAMPLE_IRQ_SOURCE_ID,
			    resampler->notifier.gsi, 0, false);
		kfree(resampler);
	}

	mutex_unlock(&kvm->irqfds.resampler_lock);
}
