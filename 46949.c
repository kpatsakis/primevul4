static bool vmx_xsaves_supported(void)
{
	return vmcs_config.cpu_based_2nd_exec_ctrl &
		SECONDARY_EXEC_XSAVES;
}
