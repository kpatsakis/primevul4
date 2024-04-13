int kvm_mtrr_set_msr(struct kvm_vcpu *vcpu, u32 msr, u64 data)
{
	int index;

	if (!kvm_mtrr_valid(vcpu, msr, data))
		return 1;

	index = fixed_msr_to_range_index(msr);
	if (index >= 0)
		*(u64 *)&vcpu->arch.mtrr_state.fixed_ranges[index] = data;
	else if (msr == MSR_MTRRdefType)
		vcpu->arch.mtrr_state.deftype = data;
	else if (msr == MSR_IA32_CR_PAT)
		vcpu->arch.pat = data;
	else
		set_var_mtrr_msr(vcpu, msr, data);

	update_mtrr(vcpu, msr);
	return 0;
}
