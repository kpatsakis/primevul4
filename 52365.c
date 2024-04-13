int kvm_mtrr_get_msr(struct kvm_vcpu *vcpu, u32 msr, u64 *pdata)
{
	int index;

	/* MSR_MTRRcap is a readonly MSR. */
	if (msr == MSR_MTRRcap) {
		/*
		 * SMRR = 0
		 * WC = 1
		 * FIX = 1
		 * VCNT = KVM_NR_VAR_MTRR
		 */
		*pdata = 0x500 | KVM_NR_VAR_MTRR;
		return 0;
	}

	if (!msr_mtrr_valid(msr))
		return 1;

	index = fixed_msr_to_range_index(msr);
	if (index >= 0)
		*pdata = *(u64 *)&vcpu->arch.mtrr_state.fixed_ranges[index];
	else if (msr == MSR_MTRRdefType)
		*pdata = vcpu->arch.mtrr_state.deftype;
	else if (msr == MSR_IA32_CR_PAT)
		*pdata = vcpu->arch.pat;
	else {	/* Variable MTRRs */
		int is_mtrr_mask;

		index = (msr - 0x200) / 2;
		is_mtrr_mask = msr - 0x200 - 2 * index;
		if (!is_mtrr_mask)
			*pdata = vcpu->arch.mtrr_state.var_ranges[index].base;
		else
			*pdata = vcpu->arch.mtrr_state.var_ranges[index].mask;

		*pdata &= (1ULL << cpuid_maxphyaddr(vcpu)) - 1;
	}

	return 0;
}
