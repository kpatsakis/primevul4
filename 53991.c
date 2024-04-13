static void load_xsave(struct kvm_vcpu *vcpu, u8 *src)
{
	struct xregs_state *xsave = &vcpu->arch.guest_fpu.state.xsave;
	u64 xstate_bv = *(u64 *)(src + XSAVE_HDR_OFFSET);
	u64 valid;

	/*
	 * Copy legacy XSAVE area, to avoid complications with CPUID
	 * leaves 0 and 1 in the loop below.
	 */
	memcpy(xsave, src, XSAVE_HDR_OFFSET);

	/* Set XSTATE_BV and possibly XCOMP_BV.  */
	xsave->header.xfeatures = xstate_bv;
	if (cpu_has_xsaves)
		xsave->header.xcomp_bv = host_xcr0 | XSTATE_COMPACTION_ENABLED;

	/*
	 * Copy each region from the non-compacted offset to the
	 * possibly compacted offset.
	 */
	valid = xstate_bv & ~XFEATURE_MASK_FPSSE;
	while (valid) {
		u64 feature = valid & -valid;
		int index = fls64(feature) - 1;
		void *dest = get_xsave_addr(xsave, feature);

		if (dest) {
			u32 size, offset, ecx, edx;
			cpuid_count(XSTATE_CPUID, index,
				    &size, &offset, &ecx, &edx);
			memcpy(dest, src + offset, size);
		}

		valid -= feature;
	}
}
