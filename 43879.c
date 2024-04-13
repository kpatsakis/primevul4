static int nested_vmx_run(struct kvm_vcpu *vcpu, bool launch)
{
	struct vmcs12 *vmcs12;
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	int cpu;
	struct loaded_vmcs *vmcs02;
	bool ia32e;

	if (!nested_vmx_check_permission(vcpu) ||
	    !nested_vmx_check_vmcs12(vcpu))
		return 1;

	skip_emulated_instruction(vcpu);
	vmcs12 = get_vmcs12(vcpu);

	if (enable_shadow_vmcs)
		copy_shadow_to_vmcs12(vmx);

	/*
	 * The nested entry process starts with enforcing various prerequisites
	 * on vmcs12 as required by the Intel SDM, and act appropriately when
	 * they fail: As the SDM explains, some conditions should cause the
	 * instruction to fail, while others will cause the instruction to seem
	 * to succeed, but return an EXIT_REASON_INVALID_STATE.
	 * To speed up the normal (success) code path, we should avoid checking
	 * for misconfigurations which will anyway be caught by the processor
	 * when using the merged vmcs02.
	 */
	if (vmcs12->launch_state == launch) {
		nested_vmx_failValid(vcpu,
			launch ? VMXERR_VMLAUNCH_NONCLEAR_VMCS
			       : VMXERR_VMRESUME_NONLAUNCHED_VMCS);
		return 1;
	}

	if (vmcs12->guest_activity_state != GUEST_ACTIVITY_ACTIVE) {
		nested_vmx_failValid(vcpu, VMXERR_ENTRY_INVALID_CONTROL_FIELD);
		return 1;
	}

	if ((vmcs12->cpu_based_vm_exec_control & CPU_BASED_USE_MSR_BITMAPS) &&
			!IS_ALIGNED(vmcs12->msr_bitmap, PAGE_SIZE)) {
		/*TODO: Also verify bits beyond physical address width are 0*/
		nested_vmx_failValid(vcpu, VMXERR_ENTRY_INVALID_CONTROL_FIELD);
		return 1;
	}

	if (nested_cpu_has2(vmcs12, SECONDARY_EXEC_VIRTUALIZE_APIC_ACCESSES) &&
			!IS_ALIGNED(vmcs12->apic_access_addr, PAGE_SIZE)) {
		/*TODO: Also verify bits beyond physical address width are 0*/
		nested_vmx_failValid(vcpu, VMXERR_ENTRY_INVALID_CONTROL_FIELD);
		return 1;
	}

	if (vmcs12->vm_entry_msr_load_count > 0 ||
	    vmcs12->vm_exit_msr_load_count > 0 ||
	    vmcs12->vm_exit_msr_store_count > 0) {
		pr_warn_ratelimited("%s: VMCS MSR_{LOAD,STORE} unsupported\n",
				    __func__);
		nested_vmx_failValid(vcpu, VMXERR_ENTRY_INVALID_CONTROL_FIELD);
		return 1;
	}

	if (!vmx_control_verify(vmcs12->cpu_based_vm_exec_control,
	      nested_vmx_procbased_ctls_low, nested_vmx_procbased_ctls_high) ||
	    !vmx_control_verify(vmcs12->secondary_vm_exec_control,
	      nested_vmx_secondary_ctls_low, nested_vmx_secondary_ctls_high) ||
	    !vmx_control_verify(vmcs12->pin_based_vm_exec_control,
	      nested_vmx_pinbased_ctls_low, nested_vmx_pinbased_ctls_high) ||
	    !vmx_control_verify(vmcs12->vm_exit_controls,
	      nested_vmx_exit_ctls_low, nested_vmx_exit_ctls_high) ||
	    !vmx_control_verify(vmcs12->vm_entry_controls,
	      nested_vmx_entry_ctls_low, nested_vmx_entry_ctls_high))
	{
		nested_vmx_failValid(vcpu, VMXERR_ENTRY_INVALID_CONTROL_FIELD);
		return 1;
	}

	if (((vmcs12->host_cr0 & VMXON_CR0_ALWAYSON) != VMXON_CR0_ALWAYSON) ||
	    ((vmcs12->host_cr4 & VMXON_CR4_ALWAYSON) != VMXON_CR4_ALWAYSON)) {
		nested_vmx_failValid(vcpu,
			VMXERR_ENTRY_INVALID_HOST_STATE_FIELD);
		return 1;
	}

	if (((vmcs12->guest_cr0 & VMXON_CR0_ALWAYSON) != VMXON_CR0_ALWAYSON) ||
	    ((vmcs12->guest_cr4 & VMXON_CR4_ALWAYSON) != VMXON_CR4_ALWAYSON)) {
		nested_vmx_entry_failure(vcpu, vmcs12,
			EXIT_REASON_INVALID_STATE, ENTRY_FAIL_DEFAULT);
		return 1;
	}
	if (vmcs12->vmcs_link_pointer != -1ull) {
		nested_vmx_entry_failure(vcpu, vmcs12,
			EXIT_REASON_INVALID_STATE, ENTRY_FAIL_VMCS_LINK_PTR);
		return 1;
	}

	/*
	 * If the load IA32_EFER VM-entry control is 1, the following checks
	 * are performed on the field for the IA32_EFER MSR:
	 * - Bits reserved in the IA32_EFER MSR must be 0.
	 * - Bit 10 (corresponding to IA32_EFER.LMA) must equal the value of
	 *   the IA-32e mode guest VM-exit control. It must also be identical
	 *   to bit 8 (LME) if bit 31 in the CR0 field (corresponding to
	 *   CR0.PG) is 1.
	 */
	if (vmcs12->vm_entry_controls & VM_ENTRY_LOAD_IA32_EFER) {
		ia32e = (vmcs12->vm_entry_controls & VM_ENTRY_IA32E_MODE) != 0;
		if (!kvm_valid_efer(vcpu, vmcs12->guest_ia32_efer) ||
		    ia32e != !!(vmcs12->guest_ia32_efer & EFER_LMA) ||
		    ((vmcs12->guest_cr0 & X86_CR0_PG) &&
		     ia32e != !!(vmcs12->guest_ia32_efer & EFER_LME))) {
			nested_vmx_entry_failure(vcpu, vmcs12,
				EXIT_REASON_INVALID_STATE, ENTRY_FAIL_DEFAULT);
			return 1;
		}
	}

	/*
	 * If the load IA32_EFER VM-exit control is 1, bits reserved in the
	 * IA32_EFER MSR must be 0 in the field for that register. In addition,
	 * the values of the LMA and LME bits in the field must each be that of
	 * the host address-space size VM-exit control.
	 */
	if (vmcs12->vm_exit_controls & VM_EXIT_LOAD_IA32_EFER) {
		ia32e = (vmcs12->vm_exit_controls &
			 VM_EXIT_HOST_ADDR_SPACE_SIZE) != 0;
		if (!kvm_valid_efer(vcpu, vmcs12->host_ia32_efer) ||
		    ia32e != !!(vmcs12->host_ia32_efer & EFER_LMA) ||
		    ia32e != !!(vmcs12->host_ia32_efer & EFER_LME)) {
			nested_vmx_entry_failure(vcpu, vmcs12,
				EXIT_REASON_INVALID_STATE, ENTRY_FAIL_DEFAULT);
			return 1;
		}
	}

	/*
	 * We're finally done with prerequisite checking, and can start with
	 * the nested entry.
	 */

	vmcs02 = nested_get_current_vmcs02(vmx);
	if (!vmcs02)
		return -ENOMEM;

	enter_guest_mode(vcpu);

	vmx->nested.vmcs01_tsc_offset = vmcs_read64(TSC_OFFSET);

	cpu = get_cpu();
	vmx->loaded_vmcs = vmcs02;
	vmx_vcpu_put(vcpu);
	vmx_vcpu_load(vcpu, cpu);
	vcpu->cpu = cpu;
	put_cpu();

	vmx_segment_cache_clear(vmx);

	vmcs12->launch_state = 1;

	prepare_vmcs02(vcpu, vmcs12);

	/*
	 * Note no nested_vmx_succeed or nested_vmx_fail here. At this point
	 * we are no longer running L1, and VMLAUNCH/VMRESUME has not yet
	 * returned as far as L1 is concerned. It will only return (and set
	 * the success flag) when L2 exits (see nested_vmx_vmexit()).
	 */
	return 1;
}
