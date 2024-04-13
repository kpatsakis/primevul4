static int vmx_set_vmx_msr(struct kvm_vcpu *vcpu, struct msr_data *msr_info)
{
	u32 msr_index = msr_info->index;
	u64 data = msr_info->data;
	bool host_initialized = msr_info->host_initiated;

	if (!nested_vmx_allowed(vcpu))
		return 0;

	if (msr_index == MSR_IA32_FEATURE_CONTROL) {
		if (!host_initialized &&
				to_vmx(vcpu)->nested.msr_ia32_feature_control
				& FEATURE_CONTROL_LOCKED)
			return 0;
		to_vmx(vcpu)->nested.msr_ia32_feature_control = data;
		return 1;
	}

	/*
	 * No need to treat VMX capability MSRs specially: If we don't handle
	 * them, handle_wrmsr will #GP(0), which is correct (they are readonly)
	 */
	return 0;
}
