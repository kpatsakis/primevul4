static int enter_vmx_non_root_mode(struct kvm_vcpu *vcpu, bool from_vmentry)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	struct vmcs12 *vmcs12 = get_vmcs12(vcpu);
	struct loaded_vmcs *vmcs02;
	u32 msr_entry_idx;
	u32 exit_qual;

	vmcs02 = nested_get_current_vmcs02(vmx);
	if (!vmcs02)
		return -ENOMEM;

	enter_guest_mode(vcpu);

	if (!(vmcs12->vm_entry_controls & VM_ENTRY_LOAD_DEBUG_CONTROLS))
		vmx->nested.vmcs01_debugctl = vmcs_read64(GUEST_IA32_DEBUGCTL);

	vmx_switch_vmcs(vcpu, vmcs02);
	vmx_segment_cache_clear(vmx);

	if (prepare_vmcs02(vcpu, vmcs12, from_vmentry, &exit_qual)) {
		leave_guest_mode(vcpu);
		vmx_switch_vmcs(vcpu, &vmx->vmcs01);
		nested_vmx_entry_failure(vcpu, vmcs12,
					 EXIT_REASON_INVALID_STATE, exit_qual);
		return 1;
	}

	nested_get_vmcs12_pages(vcpu, vmcs12);

	msr_entry_idx = nested_vmx_load_msr(vcpu,
					    vmcs12->vm_entry_msr_load_addr,
					    vmcs12->vm_entry_msr_load_count);
	if (msr_entry_idx) {
		leave_guest_mode(vcpu);
		vmx_switch_vmcs(vcpu, &vmx->vmcs01);
		nested_vmx_entry_failure(vcpu, vmcs12,
				EXIT_REASON_MSR_LOAD_FAIL, msr_entry_idx);
		return 1;
	}

	/*
	 * Note no nested_vmx_succeed or nested_vmx_fail here. At this point
	 * we are no longer running L1, and VMLAUNCH/VMRESUME has not yet
	 * returned as far as L1 is concerned. It will only return (and set
	 * the success flag) when L2 exits (see nested_vmx_vmexit()).
	 */
	return 0;
}
