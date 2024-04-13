static int check_vmentry_prereqs(struct kvm_vcpu *vcpu, struct vmcs12 *vmcs12)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	if (vmcs12->guest_activity_state != GUEST_ACTIVITY_ACTIVE &&
	    vmcs12->guest_activity_state != GUEST_ACTIVITY_HLT)
		return VMXERR_ENTRY_INVALID_CONTROL_FIELD;

	if (nested_vmx_check_io_bitmap_controls(vcpu, vmcs12))
		return VMXERR_ENTRY_INVALID_CONTROL_FIELD;

	if (nested_vmx_check_msr_bitmap_controls(vcpu, vmcs12))
		return VMXERR_ENTRY_INVALID_CONTROL_FIELD;

	if (nested_vmx_check_tpr_shadow_controls(vcpu, vmcs12))
		return VMXERR_ENTRY_INVALID_CONTROL_FIELD;

	if (nested_vmx_check_apicv_controls(vcpu, vmcs12))
		return VMXERR_ENTRY_INVALID_CONTROL_FIELD;

	if (nested_vmx_check_msr_switch_controls(vcpu, vmcs12))
		return VMXERR_ENTRY_INVALID_CONTROL_FIELD;

	if (nested_vmx_check_pml_controls(vcpu, vmcs12))
		return VMXERR_ENTRY_INVALID_CONTROL_FIELD;

	if (!vmx_control_verify(vmcs12->cpu_based_vm_exec_control,
				vmx->nested.nested_vmx_procbased_ctls_low,
				vmx->nested.nested_vmx_procbased_ctls_high) ||
	    (nested_cpu_has(vmcs12, CPU_BASED_ACTIVATE_SECONDARY_CONTROLS) &&
	     !vmx_control_verify(vmcs12->secondary_vm_exec_control,
				 vmx->nested.nested_vmx_secondary_ctls_low,
				 vmx->nested.nested_vmx_secondary_ctls_high)) ||
	    !vmx_control_verify(vmcs12->pin_based_vm_exec_control,
				vmx->nested.nested_vmx_pinbased_ctls_low,
				vmx->nested.nested_vmx_pinbased_ctls_high) ||
	    !vmx_control_verify(vmcs12->vm_exit_controls,
				vmx->nested.nested_vmx_exit_ctls_low,
				vmx->nested.nested_vmx_exit_ctls_high) ||
	    !vmx_control_verify(vmcs12->vm_entry_controls,
				vmx->nested.nested_vmx_entry_ctls_low,
				vmx->nested.nested_vmx_entry_ctls_high))
		return VMXERR_ENTRY_INVALID_CONTROL_FIELD;

	if (nested_cpu_has_vmfunc(vmcs12)) {
		if (vmcs12->vm_function_control &
		    ~vmx->nested.nested_vmx_vmfunc_controls)
			return VMXERR_ENTRY_INVALID_CONTROL_FIELD;

		if (nested_cpu_has_eptp_switching(vmcs12)) {
			if (!nested_cpu_has_ept(vmcs12) ||
			    !page_address_valid(vcpu, vmcs12->eptp_list_address))
				return VMXERR_ENTRY_INVALID_CONTROL_FIELD;
		}
	}

	if (vmcs12->cr3_target_count > nested_cpu_vmx_misc_cr3_count(vcpu))
		return VMXERR_ENTRY_INVALID_CONTROL_FIELD;

	if (!nested_host_cr0_valid(vcpu, vmcs12->host_cr0) ||
	    !nested_host_cr4_valid(vcpu, vmcs12->host_cr4) ||
	    !nested_cr3_valid(vcpu, vmcs12->host_cr3))
		return VMXERR_ENTRY_INVALID_HOST_STATE_FIELD;

	return 0;
}
