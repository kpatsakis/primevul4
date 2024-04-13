static int nested_vmx_load_msr_check(struct kvm_vcpu *vcpu,
				     struct vmx_msr_entry *e)
{
	if (e->index == MSR_FS_BASE ||
	    e->index == MSR_GS_BASE ||
	    e->index == MSR_IA32_SMM_MONITOR_CTL || /* SMM is not supported */
	    nested_vmx_msr_check_common(vcpu, e))
		return -EINVAL;
	return 0;
}
