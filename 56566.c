static int check_vmentry_postreqs(struct kvm_vcpu *vcpu, struct vmcs12 *vmcs12,
				  u32 *exit_qual)
{
	bool ia32e;

	*exit_qual = ENTRY_FAIL_DEFAULT;

	if (!nested_guest_cr0_valid(vcpu, vmcs12->guest_cr0) ||
	    !nested_guest_cr4_valid(vcpu, vmcs12->guest_cr4))
		return 1;

	if (!nested_cpu_has2(vmcs12, SECONDARY_EXEC_SHADOW_VMCS) &&
	    vmcs12->vmcs_link_pointer != -1ull) {
		*exit_qual = ENTRY_FAIL_VMCS_LINK_PTR;
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
	if (to_vmx(vcpu)->nested.nested_run_pending &&
	    (vmcs12->vm_entry_controls & VM_ENTRY_LOAD_IA32_EFER)) {
		ia32e = (vmcs12->vm_entry_controls & VM_ENTRY_IA32E_MODE) != 0;
		if (!kvm_valid_efer(vcpu, vmcs12->guest_ia32_efer) ||
		    ia32e != !!(vmcs12->guest_ia32_efer & EFER_LMA) ||
		    ((vmcs12->guest_cr0 & X86_CR0_PG) &&
		     ia32e != !!(vmcs12->guest_ia32_efer & EFER_LME)))
			return 1;
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
		    ia32e != !!(vmcs12->host_ia32_efer & EFER_LME))
			return 1;
	}

	return 0;
}
