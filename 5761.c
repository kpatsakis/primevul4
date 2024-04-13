update_mem_attrs(uintptr_t addr, uint64_t size,
		 uint64_t set_attrs, uint64_t clear_attrs)
{
	EFI_MEMORY_ATTRIBUTE_PROTOCOL *proto = NULL;
	EFI_PHYSICAL_ADDRESS physaddr = addr;
	EFI_STATUS efi_status, ret;
	uint64_t before = 0, after = 0, uefi_set_attrs, uefi_clear_attrs;

	efi_status = LibLocateProtocol(&EFI_MEMORY_ATTRIBUTE_PROTOCOL_GUID,
				       (VOID **)&proto);
	if (EFI_ERROR(efi_status) || !proto)
		return efi_status;

	efi_status = get_mem_attrs (addr, size, &before);
	if (EFI_ERROR(efi_status))
		dprint(L"get_mem_attrs(0x%llx, 0x%llx, 0x%llx) -> 0x%lx\n",
		       (unsigned long long)addr, (unsigned long long)size,
		       &before, efi_status);

	if (physaddr & 0xfff || size & 0xfff || size == 0) {
		dprint(L"%a called on 0x%llx-0x%llx (size 0x%llx) +%a%a%a -%a%a%a\n",
		       __func__, (unsigned long long)physaddr,
		       (unsigned long long)(physaddr + size - 1),
		       (unsigned long long)size,
		       (set_attrs & MEM_ATTR_R) ? "r" : "",
		       (set_attrs & MEM_ATTR_W) ? "w" : "",
		       (set_attrs & MEM_ATTR_X) ? "x" : "",
		       (clear_attrs & MEM_ATTR_R) ? "r" : "",
		       (clear_attrs & MEM_ATTR_W) ? "w" : "",
		       (clear_attrs & MEM_ATTR_X) ? "x" : "");
		return 0;
	}

	uefi_set_attrs = shim_mem_attrs_to_uefi_mem_attrs (set_attrs);
	dprint("translating set_attrs from 0x%lx to 0x%lx\n", set_attrs, uefi_set_attrs);
	uefi_clear_attrs = shim_mem_attrs_to_uefi_mem_attrs (clear_attrs);
	dprint("translating clear_attrs from 0x%lx to 0x%lx\n", clear_attrs, uefi_clear_attrs);
	efi_status = EFI_SUCCESS;
	if (uefi_set_attrs)
		efi_status = proto->SetMemoryAttributes(proto, physaddr, size, uefi_set_attrs);
	if (!EFI_ERROR(efi_status) && uefi_clear_attrs)
		efi_status = proto->ClearMemoryAttributes(proto, physaddr, size, uefi_clear_attrs);
	ret = efi_status;

	efi_status = get_mem_attrs (addr, size, &after);
	if (EFI_ERROR(efi_status))
		dprint(L"get_mem_attrs(0x%llx, %llu, 0x%llx) -> 0x%lx\n",
		       (unsigned long long)addr, (unsigned long long)size,
		       &after, efi_status);

	dprint(L"set +%a%a%a -%a%a%a on 0x%llx-0x%llx before:%c%c%c after:%c%c%c\n",
	       (set_attrs & MEM_ATTR_R) ? "r" : "",
	       (set_attrs & MEM_ATTR_W) ? "w" : "",
	       (set_attrs & MEM_ATTR_X) ? "x" : "",
	       (clear_attrs & MEM_ATTR_R) ? "r" : "",
	       (clear_attrs & MEM_ATTR_W) ? "w" : "",
	       (clear_attrs & MEM_ATTR_X) ? "x" : "",
	       (unsigned long long)addr, (unsigned long long)(addr + size - 1),
	       (before & MEM_ATTR_R) ? 'r' : '-',
	       (before & MEM_ATTR_W) ? 'w' : '-',
	       (before & MEM_ATTR_X) ? 'x' : '-',
	       (after & MEM_ATTR_R) ? 'r' : '-',
	       (after & MEM_ATTR_W) ? 'w' : '-',
	       (after & MEM_ATTR_X) ? 'x' : '-');

	return ret;
}