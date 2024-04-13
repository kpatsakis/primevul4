shim_mem_attrs_to_uefi_mem_attrs (uint64_t attrs)
{
	uint64_t ret = EFI_MEMORY_RP |
		       EFI_MEMORY_RO |
		       EFI_MEMORY_XP;

	if (attrs & MEM_ATTR_R)
		ret &= ~EFI_MEMORY_RP;

	if (attrs & MEM_ATTR_W)
		ret &= ~EFI_MEMORY_RO;

	if (attrs & MEM_ATTR_X)
		ret &= ~EFI_MEMORY_XP;

	return ret;
}