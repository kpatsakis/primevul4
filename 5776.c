get_mem_attrs (uintptr_t addr, size_t size, uint64_t *attrs)
{
	EFI_MEMORY_ATTRIBUTE_PROTOCOL *proto = NULL;
	EFI_PHYSICAL_ADDRESS physaddr = addr;
	EFI_STATUS efi_status;

	efi_status = LibLocateProtocol(&EFI_MEMORY_ATTRIBUTE_PROTOCOL_GUID,
				       (VOID **)&proto);
	if (EFI_ERROR(efi_status) || !proto)
		return efi_status;

	if (physaddr & 0xfff || size & 0xfff || size == 0 || attrs == NULL) {
		dprint(L"%a called on 0x%llx-0x%llx and attrs 0x%llx\n",
		       __func__, (unsigned long long)physaddr,
		       (unsigned long long)(physaddr+size-1),
		       attrs);
		return EFI_SUCCESS;
	}

	efi_status = proto->GetMemoryAttributes(proto, physaddr, size, attrs);
	*attrs = uefi_mem_attrs_to_shim_mem_attrs (*attrs);

	return efi_status;
}