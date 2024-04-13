static inline bool cpu_has_vmx_ept_5levels(void)
{
	return vmx_capability.ept & VMX_EPT_PAGE_WALK_5_BIT;
}
