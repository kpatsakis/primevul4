void kvm_put_guest_fpu(struct kvm_vcpu *vcpu)
{
	kvm_put_guest_xcr0(vcpu);

	if (!vcpu->guest_fpu_loaded) {
		vcpu->fpu_counter = 0;
		return;
	}

	vcpu->guest_fpu_loaded = 0;
	copy_fpregs_to_fpstate(&vcpu->arch.guest_fpu);
	__kernel_fpu_end();
	++vcpu->stat.fpu_reload;
	/*
	 * If using eager FPU mode, or if the guest is a frequent user
	 * of the FPU, just leave the FPU active for next time.
	 * Every 255 times fpu_counter rolls over to 0; a guest that uses
	 * the FPU in bursts will revert to loading it on demand.
	 */
	if (!vcpu->arch.eager_fpu) {
		if (++vcpu->fpu_counter < 5)
			kvm_make_request(KVM_REQ_DEACTIVATE_FPU, vcpu);
	}
	trace_kvm_fpu(0);
}
