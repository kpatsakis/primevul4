static void vmx_set_virtual_x2apic_mode(struct kvm_vcpu *vcpu, bool set)
{
	u32 sec_exec_control;

	/* Postpone execution until vmcs01 is the current VMCS. */
	if (is_guest_mode(vcpu)) {
		to_vmx(vcpu)->nested.change_vmcs01_virtual_x2apic_mode = true;
		return;
	}

	if (!cpu_has_vmx_virtualize_x2apic_mode())
		return;

	if (!cpu_need_tpr_shadow(vcpu))
		return;

	sec_exec_control = vmcs_read32(SECONDARY_VM_EXEC_CONTROL);

	if (set) {
		sec_exec_control &= ~SECONDARY_EXEC_VIRTUALIZE_APIC_ACCESSES;
		sec_exec_control |= SECONDARY_EXEC_VIRTUALIZE_X2APIC_MODE;
	} else {
		sec_exec_control &= ~SECONDARY_EXEC_VIRTUALIZE_X2APIC_MODE;
		sec_exec_control |= SECONDARY_EXEC_VIRTUALIZE_APIC_ACCESSES;
		vmx_flush_tlb_ept_only(vcpu);
	}
	vmcs_write32(SECONDARY_VM_EXEC_CONTROL, sec_exec_control);

	vmx_set_msr_bitmap(vcpu);
}
