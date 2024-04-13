static void update_mtrr(struct kvm_vcpu *vcpu, u32 msr)
{
	struct kvm_mtrr *mtrr_state = &vcpu->arch.mtrr_state;
	gfn_t start, end;
	int index;

	if (msr == MSR_IA32_CR_PAT || !tdp_enabled ||
	      !kvm_arch_has_noncoherent_dma(vcpu->kvm))
		return;

	if (!mtrr_is_enabled(mtrr_state) && msr != MSR_MTRRdefType)
		return;

	/* fixed MTRRs. */
	if (fixed_msr_to_range(msr, &start, &end)) {
		if (!fixed_mtrr_is_enabled(mtrr_state))
			return;
	} else if (msr == MSR_MTRRdefType) {
		start = 0x0;
		end = ~0ULL;
	} else {
		/* variable range MTRRs. */
		index = (msr - 0x200) / 2;
		var_mtrr_range(&mtrr_state->var_ranges[index], &start, &end);
	}

	kvm_zap_gfn_range(vcpu->kvm, gpa_to_gfn(start), gpa_to_gfn(end));
}
