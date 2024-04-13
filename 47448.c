int pit_has_pending_timer(struct kvm_vcpu *vcpu)
{
	struct kvm_pit *pit = vcpu->kvm->arch.vpit;

	if (pit && kvm_vcpu_is_bsp(vcpu) && pit->pit_state.irq_ack)
		return atomic_read(&pit->pit_state.pit_timer.pending);
	return 0;
}
