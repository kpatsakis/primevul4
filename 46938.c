static void vmx_set_virtual_x2apic_mode(struct kvm_vcpu *vcpu, bool set)
{
	u32 sec_exec_control;

	/*
	 * There is not point to enable virtualize x2apic without enable
	 * apicv
	 */
	if (!cpu_has_vmx_virtualize_x2apic_mode() ||
				!vmx_cpu_uses_apicv(vcpu))
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
	}
	vmcs_write32(SECONDARY_VM_EXEC_CONTROL, sec_exec_control);

	vmx_set_msr_bitmap(vcpu);
}
