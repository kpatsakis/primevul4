bool kvm_irq_has_notifier(struct kvm *kvm, unsigned irqchip, unsigned pin)
{
	struct kvm_irq_ack_notifier *kian;
	int gsi, idx;

	idx = srcu_read_lock(&kvm->irq_srcu);
	gsi = kvm_irq_map_chip_pin(kvm, irqchip, pin);
	if (gsi != -1)
		hlist_for_each_entry_rcu(kian, &kvm->irq_ack_notifier_list,
					 link)
			if (kian->gsi == gsi) {
				srcu_read_unlock(&kvm->irq_srcu, idx);
				return true;
			}

	srcu_read_unlock(&kvm->irq_srcu, idx);

	return false;
}
