uefi_mem_attrs_to_shim_mem_attrs (uint64_t attrs)
{
	uint64_t ret = MEM_ATTR_R |
		       MEM_ATTR_W |
		       MEM_ATTR_X;

	if (attrs & EFI_MEMORY_RP)
		ret &= ~MEM_ATTR_R;

	if (attrs & EFI_MEMORY_RO)
		ret &= ~MEM_ATTR_W;

	if (attrs & EFI_MEMORY_XP)
		ret &= ~MEM_ATTR_X;

	return ret;
}