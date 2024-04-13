static inline bool cpu_has_vmx_ept_mt_wb(void)
{
	return vmx_capability.ept & VMX_EPTP_WB_BIT;
}
