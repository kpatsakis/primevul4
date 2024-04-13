static int nested_vmx_check_msr_switch_controls(struct kvm_vcpu *vcpu,
						struct vmcs12 *vmcs12)
{
	if (vmcs12->vm_exit_msr_load_count == 0 &&
	    vmcs12->vm_exit_msr_store_count == 0 &&
	    vmcs12->vm_entry_msr_load_count == 0)
		return 0; /* Fast path */
	if (nested_vmx_check_msr_switch(vcpu, VM_EXIT_MSR_LOAD_COUNT,
					VM_EXIT_MSR_LOAD_ADDR) ||
	    nested_vmx_check_msr_switch(vcpu, VM_EXIT_MSR_STORE_COUNT,
					VM_EXIT_MSR_STORE_ADDR) ||
	    nested_vmx_check_msr_switch(vcpu, VM_ENTRY_MSR_LOAD_COUNT,
					VM_ENTRY_MSR_LOAD_ADDR))
		return -EINVAL;
	return 0;
}
