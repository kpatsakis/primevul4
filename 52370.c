static u8 mtrr_disabled_type(struct kvm_vcpu *vcpu)
{
	/*
	 * Intel SDM 11.11.2.2: all MTRRs are disabled when
	 * IA32_MTRR_DEF_TYPE.E bit is cleared, and the UC
	 * memory type is applied to all of physical memory.
	 *
	 * However, virtual machines can be run with CPUID such that
	 * there are no MTRRs.  In that case, the firmware will never
	 * enable MTRRs and it is obviously undesirable to run the
	 * guest entirely with UC memory and we use WB.
	 */
	if (guest_cpuid_has_mtrr(vcpu))
		return MTRR_TYPE_UNCACHABLE;
	else
		return MTRR_TYPE_WRBACK;
}
