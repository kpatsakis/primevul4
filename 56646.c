static void vmx_disable_shadow_vmcs(struct vcpu_vmx *vmx)
{
	vmcs_clear_bits(SECONDARY_VM_EXEC_CONTROL, SECONDARY_EXEC_SHADOW_VMCS);
	vmcs_write64(VMCS_LINK_POINTER, -1ull);
}
