void kvm_notify_acked_gsi(struct kvm *kvm, int gsi)
{
	struct kvm_irq_ack_notifier *kian;

	hlist_for_each_entry_rcu(kian, &kvm->irq_ack_notifier_list,
				 link)
		if (kian->gsi == gsi)
			kian->irq_acked(kian);
}
