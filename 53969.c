int kvm_vcpu_halt(struct kvm_vcpu *vcpu)
{
	++vcpu->stat.halt_exits;
	if (lapic_in_kernel(vcpu)) {
		vcpu->arch.mp_state = KVM_MP_STATE_HALTED;
		return 1;
	} else {
		vcpu->run->exit_reason = KVM_EXIT_HLT;
		return 0;
	}
}
