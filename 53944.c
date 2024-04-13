void kvm_load_guest_fpu(struct kvm_vcpu *vcpu)
{
	if (vcpu->guest_fpu_loaded)
		return;

	/*
	 * Restore all possible states in the guest,
	 * and assume host would use all available bits.
	 * Guest xcr0 would be loaded later.
	 */
	kvm_put_guest_xcr0(vcpu);
	vcpu->guest_fpu_loaded = 1;
	__kernel_fpu_begin();
	__copy_kernel_to_fpregs(&vcpu->arch.guest_fpu.state);
	trace_kvm_fpu(1);
}
