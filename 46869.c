static inline bool nested_cpu_has_xsaves(struct vmcs12 *vmcs12)
{
	return nested_cpu_has2(vmcs12, SECONDARY_EXEC_XSAVES) &&
		vmx_xsaves_supported();
}
