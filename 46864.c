static inline bool nested_cpu_has_apic_reg_virt(struct vmcs12 *vmcs12)
{
	return nested_cpu_has2(vmcs12, SECONDARY_EXEC_APIC_REGISTER_VIRT);
}
