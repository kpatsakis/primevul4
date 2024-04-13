void kvm_unregister_irq_ack_notifier(struct kvm *kvm,
				    struct kvm_irq_ack_notifier *kian)
{
	mutex_lock(&kvm->irq_lock);
	hlist_del_init_rcu(&kian->link);
	mutex_unlock(&kvm->irq_lock);
	synchronize_srcu(&kvm->irq_srcu);
	kvm_arch_post_irq_ack_notifier_list_update(kvm);
}
