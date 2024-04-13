verify_sbat_section(char *SBATBase, size_t SBATSize)
{
	unsigned int i;
	EFI_STATUS efi_status;
	size_t n;
	struct sbat_section_entry **entries = NULL;
	char *sbat_data;
	size_t sbat_size;

	if (list_empty(&sbat_var))
		return EFI_SUCCESS;

	if (SBATBase == NULL || SBATSize == 0) {
		dprint(L"No .sbat section data\n");
		/*
		 * SBAT is mandatory for binaries loaded by shim, but optional
		 * for binaries loaded outside of shim but verified via the
		 * protocol.
		 */
		return in_protocol ? EFI_SUCCESS : EFI_SECURITY_VIOLATION;
	}

	sbat_size = SBATSize + 1;
	sbat_data = AllocatePool(sbat_size);
	if (!sbat_data) {
		console_print(L"Failed to allocate .sbat section buffer\n");
		return EFI_OUT_OF_RESOURCES;
	}
	CopyMem(sbat_data, SBATBase, SBATSize);
	sbat_data[SBATSize] = '\0';

	efi_status = parse_sbat_section(sbat_data, sbat_size, &n, &entries);
	if (EFI_ERROR(efi_status)) {
		perror(L"Could not parse .sbat section data: %r\n", efi_status);
		goto err;
	}

	dprint(L"SBAT section data\n");
        for (i = 0; i < n; i++) {
		dprint(L"%a, %a, %a, %a, %a, %a\n",
		       entries[i]->component_name,
		       entries[i]->component_generation,
		       entries[i]->vendor_name,
		       entries[i]->vendor_package_name,
		       entries[i]->vendor_version,
		       entries[i]->vendor_url);
	}

	efi_status = verify_sbat(n, entries);

	cleanup_sbat_section_entries(n, entries);

err:
	FreePool(sbat_data);

	return efi_status;
}