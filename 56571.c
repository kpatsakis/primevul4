static inline bool cpu_has_vmx_invvpid(void)
{
	return vmx_capability.vpid & VMX_VPID_INVVPID_BIT;
}
