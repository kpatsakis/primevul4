static void fx_init(struct kvm_vcpu *vcpu)
{
	fpstate_init(&vcpu->arch.guest_fpu.state);
	if (cpu_has_xsaves)
		vcpu->arch.guest_fpu.state.xsave.header.xcomp_bv =
			host_xcr0 | XSTATE_COMPACTION_ENABLED;

	/*
	 * Ensure guest xcr0 is valid for loading
	 */
	vcpu->arch.xcr0 = XFEATURE_MASK_FP;

	vcpu->arch.cr0 |= X86_CR0_ET;
}
