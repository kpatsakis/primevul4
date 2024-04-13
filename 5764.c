handle_image (void *data, unsigned int datasize,
	      EFI_LOADED_IMAGE *li,
	      EFI_IMAGE_ENTRY_POINT *entry_point,
	      EFI_PHYSICAL_ADDRESS *alloc_address,
	      UINTN *alloc_pages)
{
	EFI_STATUS efi_status;
	char *buffer;
	int i;
	EFI_IMAGE_SECTION_HEADER *Section;
	char *base, *end;
	UINT32 size;
	PE_COFF_LOADER_IMAGE_CONTEXT context;
	unsigned int alignment, alloc_size;
	int found_entry_point = 0;
	UINT8 sha1hash[SHA1_DIGEST_SIZE];
	UINT8 sha256hash[SHA256_DIGEST_SIZE];

	/*
	 * The binary header contains relevant context and section pointers
	 */
	efi_status = read_header(data, datasize, &context);
	if (EFI_ERROR(efi_status)) {
		perror(L"Failed to read header: %r\n", efi_status);
		return efi_status;
	}

	/*
	 * Perform the image verification before we start copying data around
	 * in order to load it.
	 */
	if (secure_mode ()) {
		efi_status = verify_buffer(data, datasize, &context, sha256hash,
					   sha1hash);

		if (EFI_ERROR(efi_status)) {
			if (verbose)
				console_print(L"Verification failed: %r\n", efi_status);
			else
				console_error(L"Verification failed", efi_status);
			return efi_status;
		} else {
			if (verbose)
				console_print(L"Verification succeeded\n");
		}
	}

	/*
	 * Calculate the hash for the TPM measurement.
	 * XXX: We're computing these twice in secure boot mode when the
	 *  buffers already contain the previously computed hashes. Also,
	 *  this is only useful for the TPM1.2 case. We should try to fix
	 *  this in a follow-up.
	 */
	efi_status = generate_hash(data, datasize, &context, sha256hash,
				   sha1hash);
	if (EFI_ERROR(efi_status))
		return efi_status;

	/* Measure the binary into the TPM */
#ifdef REQUIRE_TPM
	efi_status =
#endif
	tpm_log_pe((EFI_PHYSICAL_ADDRESS)(UINTN)data, datasize,
		   (EFI_PHYSICAL_ADDRESS)(UINTN)context.ImageAddress,
		   li->FilePath, sha1hash, 4);
#ifdef REQUIRE_TPM
	if (efi_status != EFI_SUCCESS) {
		return efi_status;
	}
#endif

	/* The spec says, uselessly, of SectionAlignment:
	 * =====
	 * The alignment (in bytes) of sections when they are loaded into
	 * memory. It must be greater than or equal to FileAlignment. The
	 * default is the page size for the architecture.
	 * =====
	 * Which doesn't tell you whose responsibility it is to enforce the
	 * "default", or when.  It implies that the value in the field must
	 * be > FileAlignment (also poorly defined), but it appears visual
	 * studio will happily write 512 for FileAlignment (its default) and
	 * 0 for SectionAlignment, intending to imply PAGE_SIZE.
	 *
	 * We only support one page size, so if it's zero, nerf it to 4096.
	 */
	alignment = context.SectionAlignment;
	if (!alignment)
		alignment = 4096;

	alloc_size = ALIGN_VALUE(context.ImageSize + context.SectionAlignment,
				 PAGE_SIZE);
	*alloc_pages = alloc_size / PAGE_SIZE;

	efi_status = BS->AllocatePages(AllocateAnyPages, EfiLoaderCode,
				       *alloc_pages, alloc_address);
	if (EFI_ERROR(efi_status)) {
		perror(L"Failed to allocate image buffer\n");
		return EFI_OUT_OF_RESOURCES;
	}

	buffer = (void *)ALIGN_VALUE((unsigned long)*alloc_address, alignment);
	dprint(L"Loading 0x%llx bytes at 0x%llx\n",
	       (unsigned long long)context.ImageSize,
	       (unsigned long long)(uintptr_t)buffer);
	update_mem_attrs((uintptr_t)buffer, alloc_size, MEM_ATTR_R|MEM_ATTR_W,
			 MEM_ATTR_X);

	CopyMem(buffer, data, context.SizeOfHeaders);

	*entry_point = ImageAddress(buffer, context.ImageSize, context.EntryPoint);
	if (!*entry_point) {
		perror(L"Entry point is invalid\n");
		BS->FreePages(*alloc_address, *alloc_pages);
		return EFI_UNSUPPORTED;
	}

	char *RelocBase, *RelocBaseEnd;
	/*
	 * These are relative virtual addresses, so we have to check them
	 * against the image size, not the data size.
	 */
	RelocBase = ImageAddress(buffer, context.ImageSize,
				 context.RelocDir->VirtualAddress);
	/*
	 * RelocBaseEnd here is the address of the last byte of the table
	 */
	RelocBaseEnd = ImageAddress(buffer, context.ImageSize,
				    context.RelocDir->VirtualAddress +
				    context.RelocDir->Size - 1);

	EFI_IMAGE_SECTION_HEADER *RelocSection = NULL;

	/*
	 * Copy the executable's sections to their desired offsets
	 */
	Section = context.FirstSection;
	for (i = 0; i < context.NumberOfSections; i++, Section++) {
		/* Don't try to copy discardable sections with zero size */
		if ((Section->Characteristics & EFI_IMAGE_SCN_MEM_DISCARDABLE) &&
		    !Section->Misc.VirtualSize)
			continue;

		/*
		 * Skip sections that aren't marked readable.
		 */
		if (!(Section->Characteristics & EFI_IMAGE_SCN_MEM_READ))
			continue;

		if (!(Section->Characteristics & EFI_IMAGE_SCN_MEM_DISCARDABLE) &&
		    (Section->Characteristics & EFI_IMAGE_SCN_MEM_WRITE) &&
		    (Section->Characteristics & EFI_IMAGE_SCN_MEM_EXECUTE) &&
		    (mok_policy & MOK_POLICY_REQUIRE_NX)) {
			perror(L"Section %d is writable and executable\n", i);
			return EFI_UNSUPPORTED;
		}

		base = ImageAddress (buffer, context.ImageSize,
				     Section->VirtualAddress);
		end = ImageAddress (buffer, context.ImageSize,
				    Section->VirtualAddress
				     + Section->Misc.VirtualSize - 1);

		if (end < base) {
			perror(L"Section %d has negative size\n", i);
			BS->FreePages(*alloc_address, *alloc_pages);
			return EFI_UNSUPPORTED;
		}

		if (Section->VirtualAddress <= context.EntryPoint &&
		    (Section->VirtualAddress + Section->SizeOfRawData - 1)
		    > context.EntryPoint)
			found_entry_point++;

		/* We do want to process .reloc, but it's often marked
		 * discardable, so we don't want to memcpy it. */
		if (CompareMem(Section->Name, ".reloc\0\0", 8) == 0) {
			if (RelocSection) {
				perror(L"Image has multiple relocation sections\n");
				return EFI_UNSUPPORTED;
			}
			/* If it has nonzero sizes, and our bounds check
			 * made sense, and the VA and size match RelocDir's
			 * versions, then we believe in this section table. */
			if (Section->SizeOfRawData &&
					Section->Misc.VirtualSize &&
					base && end &&
					RelocBase == base &&
					RelocBaseEnd == end) {
				RelocSection = Section;
			}
		}

		if (Section->Characteristics & EFI_IMAGE_SCN_MEM_DISCARDABLE) {
			continue;
		}

		if (!base) {
			perror(L"Section %d has invalid base address\n", i);
			return EFI_UNSUPPORTED;
		}
		if (!end) {
			perror(L"Section %d has zero size\n", i);
			return EFI_UNSUPPORTED;
		}

		if (!(Section->Characteristics & EFI_IMAGE_SCN_CNT_UNINITIALIZED_DATA) &&
		    (Section->VirtualAddress < context.SizeOfHeaders ||
		     Section->PointerToRawData < context.SizeOfHeaders)) {
			perror(L"Section %d is inside image headers\n", i);
			return EFI_UNSUPPORTED;
		}

		if (Section->Characteristics & EFI_IMAGE_SCN_CNT_UNINITIALIZED_DATA) {
			ZeroMem(base, Section->Misc.VirtualSize);
		} else {
			if (Section->PointerToRawData < context.SizeOfHeaders) {
				perror(L"Section %d is inside image headers\n", i);
				return EFI_UNSUPPORTED;
			}

			size = Section->Misc.VirtualSize;
			if (size > Section->SizeOfRawData)
				size = Section->SizeOfRawData;

			if (size > 0)
				CopyMem(base, data + Section->PointerToRawData, size);

			if (size < Section->Misc.VirtualSize)
				ZeroMem(base + size, Section->Misc.VirtualSize - size);
		}
	}

	if (context.NumberOfRvaAndSizes <= EFI_IMAGE_DIRECTORY_ENTRY_BASERELOC) {
		perror(L"Image has no relocation entry\n");
		FreePool(buffer);
		return EFI_UNSUPPORTED;
	}

	if (context.RelocDir->Size && RelocSection) {
		/*
		 * Run the relocation fixups
		 */
		efi_status = relocate_coff(&context, RelocSection, data,
					   buffer);

		if (EFI_ERROR(efi_status)) {
			perror(L"Relocation failed: %r\n", efi_status);
			FreePool(buffer);
			return efi_status;
		}
	}

	/*
	 * Now set the page permissions appropriately.
	 */
	Section = context.FirstSection;
	for (i = 0; i < context.NumberOfSections; i++, Section++) {
		uint64_t set_attrs = MEM_ATTR_R;
		uint64_t clear_attrs = MEM_ATTR_W|MEM_ATTR_X;
		uintptr_t addr;
		uint64_t length;

		/*
		 * Skip discardable sections with zero size
		 */
		if ((Section->Characteristics & EFI_IMAGE_SCN_MEM_DISCARDABLE) &&
		    !Section->Misc.VirtualSize)
			continue;

		/*
		 * Skip sections that aren't marked readable.
		 */
		if (!(Section->Characteristics & EFI_IMAGE_SCN_MEM_READ))
			continue;

		base = ImageAddress (buffer, context.ImageSize,
				     Section->VirtualAddress);
		end = ImageAddress (buffer, context.ImageSize,
				    Section->VirtualAddress
				     + Section->Misc.VirtualSize - 1);

		addr = (uintptr_t)base;
		length = (uintptr_t)end - (uintptr_t)base + 1;

		if (Section->Characteristics & EFI_IMAGE_SCN_MEM_WRITE) {
			set_attrs |= MEM_ATTR_W;
			clear_attrs &= ~MEM_ATTR_W;
		}
		if (Section->Characteristics & EFI_IMAGE_SCN_MEM_EXECUTE) {
			set_attrs |= MEM_ATTR_X;
			clear_attrs &= ~MEM_ATTR_X;
		}
		update_mem_attrs(addr, length, set_attrs, clear_attrs);
	}


	/*
	 * grub needs to know its location and size in memory, so fix up
	 * the loaded image protocol values
	 */
	li->ImageBase = buffer;
	li->ImageSize = context.ImageSize;

	/* Pass the load options to the second stage loader */
	li->LoadOptions = load_options;
	li->LoadOptionsSize = load_options_size;

	if (!found_entry_point) {
		perror(L"Entry point is not within sections\n");
		return EFI_UNSUPPORTED;
	}
	if (found_entry_point > 1) {
		perror(L"%d sections contain entry point\n", found_entry_point);
		return EFI_UNSUPPORTED;
	}

	return EFI_SUCCESS;
}