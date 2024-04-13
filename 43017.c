int __kvm_set_xcr(struct kvm_vcpu *vcpu, u32 index, u64 xcr)
{
	u64 xcr0 = xcr;
	u64 old_xcr0 = vcpu->arch.xcr0;
	u64 valid_bits;

	/* Only support XCR_XFEATURE_ENABLED_MASK(xcr0) now  */
	if (index != XCR_XFEATURE_ENABLED_MASK)
		return 1;
	if (!(xcr0 & XSTATE_FP))
		return 1;
	if ((xcr0 & XSTATE_YMM) && !(xcr0 & XSTATE_SSE))
		return 1;

	/*
	 * Do not allow the guest to set bits that we do not support
	 * saving.  However, xcr0 bit 0 is always set, even if the
	 * emulated CPU does not support XSAVE (see fx_init).
	 */
	valid_bits = vcpu->arch.guest_supported_xcr0 | XSTATE_FP;
	if (xcr0 & ~valid_bits)
		return 1;

	if ((!(xcr0 & XSTATE_BNDREGS)) != (!(xcr0 & XSTATE_BNDCSR)))
		return 1;

	kvm_put_guest_xcr0(vcpu);
	vcpu->arch.xcr0 = xcr0;

	if ((xcr0 ^ old_xcr0) & XSTATE_EXTEND_MASK)
		kvm_update_cpuid(vcpu);
	return 0;
}
