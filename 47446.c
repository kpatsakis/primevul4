static void kvm_pit_ack_irq(struct kvm_irq_ack_notifier *kian)
{
	struct kvm_kpit_state *ps = container_of(kian, struct kvm_kpit_state,
						 irq_ack_notifier);
	spin_lock(&ps->inject_lock);
	if (atomic_dec_return(&ps->pit_timer.pending) < 0)
		atomic_inc(&ps->pit_timer.pending);
	ps->irq_ack = 1;
	spin_unlock(&ps->inject_lock);
}
