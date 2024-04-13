static inline bool cpu_has_vmx_posted_intr(void)
{
	return IS_ENABLED(CONFIG_X86_LOCAL_APIC) &&
		vmcs_config.pin_based_exec_ctrl & PIN_BASED_POSTED_INTR;
}
