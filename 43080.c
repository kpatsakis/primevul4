static bool mtrr_valid(struct kvm_vcpu *vcpu, u32 msr, u64 data)
{
	int i;
	u64 mask;

	if (!msr_mtrr_valid(msr))
		return false;

	if (msr == MSR_IA32_CR_PAT) {
		for (i = 0; i < 8; i++)
			if (!valid_pat_type((data >> (i * 8)) & 0xff))
				return false;
		return true;
	} else if (msr == MSR_MTRRdefType) {
		if (data & ~0xcff)
			return false;
		return valid_mtrr_type(data & 0xff);
	} else if (msr >= MSR_MTRRfix64K_00000 && msr <= MSR_MTRRfix4K_F8000) {
		for (i = 0; i < 8 ; i++)
			if (!valid_mtrr_type((data >> (i * 8)) & 0xff))
				return false;
		return true;
	}

	/* variable MTRRs */
	WARN_ON(!(msr >= 0x200 && msr < 0x200 + 2 * KVM_NR_VAR_MTRR));

	mask = (~0ULL) << cpuid_maxphyaddr(vcpu);
	if ((msr & 1) == 0) {
		/* MTRR base */
		if (!valid_mtrr_type(data & 0xff))
			return false;
		mask |= 0xf00;
	} else
		/* MTRR mask */
		mask |= 0x7ff;
	if (data & mask) {
		kvm_inject_gp(vcpu, 0);
		return false;
	}

	return true;
}
