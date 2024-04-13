void kvm_inject_pit_timer_irqs(struct kvm_vcpu *vcpu)
{
	struct kvm_pit *pit = vcpu->kvm->arch.vpit;
	struct kvm *kvm = vcpu->kvm;
	struct kvm_kpit_state *ps;

	if (pit) {
		int inject = 0;
		ps = &pit->pit_state;

		/* Try to inject pending interrupts when
		 * last one has been acked.
		 */
		spin_lock(&ps->inject_lock);
		if (atomic_read(&ps->pit_timer.pending) && ps->irq_ack) {
			ps->irq_ack = 0;
			inject = 1;
		}
		spin_unlock(&ps->inject_lock);
		if (inject)
			__inject_pit_timer_intr(kvm);
	}
}
