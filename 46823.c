static inline bool cpu_has_vmx_pml(void)
{
	return vmcs_config.cpu_based_2nd_exec_ctrl & SECONDARY_EXEC_ENABLE_PML;
}
