static void vmx_set_efer(struct kvm_vcpu *vcpu, u64 efer)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	struct shared_msr_entry *msr = find_msr_entry(vmx, MSR_EFER);

	if (!msr)
		return;

	/*
	 * Force kernel_gs_base reloading before EFER changes, as control
	 * of this msr depends on is_long_mode().
	 */
	vmx_load_host_state(to_vmx(vcpu));
	vcpu->arch.efer = efer;
	if (efer & EFER_LMA) {
		vmcs_write32(VM_ENTRY_CONTROLS,
			     vmcs_read32(VM_ENTRY_CONTROLS) |
			     VM_ENTRY_IA32E_MODE);
		msr->data = efer;
	} else {
		vmcs_write32(VM_ENTRY_CONTROLS,
			     vmcs_read32(VM_ENTRY_CONTROLS) &
			     ~VM_ENTRY_IA32E_MODE);

		msr->data = efer & ~EFER_LME;
	}
	setup_msrs(vmx);
}
