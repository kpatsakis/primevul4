generate_hash(char *data, unsigned int datasize,
	      PE_COFF_LOADER_IMAGE_CONTEXT *context, UINT8 *sha256hash,
	      UINT8 *sha1hash)
{
	unsigned int sha256ctxsize, sha1ctxsize;
	void *sha256ctx = NULL, *sha1ctx = NULL;
	char *hashbase;
	unsigned int hashsize;
	unsigned int SumOfBytesHashed, SumOfSectionBytes;
	unsigned int index, pos;
	EFI_IMAGE_SECTION_HEADER *Section;
	EFI_IMAGE_SECTION_HEADER *SectionHeader = NULL;
	EFI_STATUS efi_status = EFI_SUCCESS;
	EFI_IMAGE_DOS_HEADER *DosHdr = (void *)data;
	unsigned int PEHdr_offset = 0;

	if (datasize <= sizeof (*DosHdr) ||
	    DosHdr->e_magic != EFI_IMAGE_DOS_SIGNATURE) {
		perror(L"Invalid signature\n");
		return EFI_INVALID_PARAMETER;
	}
	PEHdr_offset = DosHdr->e_lfanew;

	sha256ctxsize = Sha256GetContextSize();
	sha256ctx = AllocatePool(sha256ctxsize);

	sha1ctxsize = Sha1GetContextSize();
	sha1ctx = AllocatePool(sha1ctxsize);

	if (!sha256ctx || !sha1ctx) {
		perror(L"Unable to allocate memory for hash context\n");
		return EFI_OUT_OF_RESOURCES;
	}

	if (!Sha256Init(sha256ctx) || !Sha1Init(sha1ctx)) {
		perror(L"Unable to initialise hash\n");
		efi_status = EFI_OUT_OF_RESOURCES;
		goto done;
	}

	/* Hash start to checksum */
	hashbase = data;
	hashsize = (char *)&context->PEHdr->Pe32.OptionalHeader.CheckSum -
		hashbase;
	check_size(data, datasize, hashbase, hashsize);

	if (!(Sha256Update(sha256ctx, hashbase, hashsize)) ||
	    !(Sha1Update(sha1ctx, hashbase, hashsize))) {
		perror(L"Unable to generate hash\n");
		efi_status = EFI_OUT_OF_RESOURCES;
		goto done;
	}

	/* Hash post-checksum to start of certificate table */
	hashbase = (char *)&context->PEHdr->Pe32.OptionalHeader.CheckSum +
		sizeof (int);
	hashsize = (char *)context->SecDir - hashbase;
	check_size(data, datasize, hashbase, hashsize);

	if (!(Sha256Update(sha256ctx, hashbase, hashsize)) ||
	    !(Sha1Update(sha1ctx, hashbase, hashsize))) {
		perror(L"Unable to generate hash\n");
		efi_status = EFI_OUT_OF_RESOURCES;
		goto done;
	}

	/* Hash end of certificate table to end of image header */
	EFI_IMAGE_DATA_DIRECTORY *dd = context->SecDir + 1;
	hashbase = (char *)dd;
	hashsize = context->SizeOfHeaders - (unsigned long)((char *)dd - data);
	if (hashsize > datasize) {
		perror(L"Data Directory size %d is invalid\n", hashsize);
		efi_status = EFI_INVALID_PARAMETER;
		goto done;
	}
	check_size(data, datasize, hashbase, hashsize);

	if (!(Sha256Update(sha256ctx, hashbase, hashsize)) ||
	    !(Sha1Update(sha1ctx, hashbase, hashsize))) {
		perror(L"Unable to generate hash\n");
		efi_status = EFI_OUT_OF_RESOURCES;
		goto done;
	}

	/* Sort sections */
	SumOfBytesHashed = context->SizeOfHeaders;

	/*
	 * XXX Do we need this here, or is it already done in all cases?
	 */
	if (context->NumberOfSections == 0 ||
	    context->FirstSection == NULL) {
		uint16_t opthdrsz;
		uint64_t addr;
		uint16_t nsections;
		EFI_IMAGE_SECTION_HEADER *section0, *sectionN;

		nsections = context->PEHdr->Pe32.FileHeader.NumberOfSections;
		opthdrsz = context->PEHdr->Pe32.FileHeader.SizeOfOptionalHeader;

		/* Validate section0 is within image */
		addr = PEHdr_offset + sizeof(UINT32)
			+ sizeof(EFI_IMAGE_FILE_HEADER)
			+ opthdrsz;
		section0 = ImageAddress(data, datasize, addr);
		if (!section0) {
			perror(L"Malformed file header.\n");
			perror(L"Image address for Section Header 0 is 0x%016llx\n",
			       addr);
			perror(L"File size is 0x%016llx\n", datasize);
			efi_status = EFI_INVALID_PARAMETER;
			goto done;
		}

		/* Validate sectionN is within image */
		addr += (uint64_t)(intptr_t)&section0[nsections-1] -
			(uint64_t)(intptr_t)section0;
		sectionN = ImageAddress(data, datasize, addr);
		if (!sectionN) {
			perror(L"Malformed file header.\n");
			perror(L"Image address for Section Header %d is 0x%016llx\n",
			       nsections - 1, addr);
			perror(L"File size is 0x%016llx\n", datasize);
			efi_status = EFI_INVALID_PARAMETER;
			goto done;
		}

		context->NumberOfSections = nsections;
		context->FirstSection = section0;
	}

	/*
	 * Allocate a new section table so we can sort them without
	 * modifying the image.
	 */
	SectionHeader = AllocateZeroPool (sizeof (EFI_IMAGE_SECTION_HEADER)
					  * context->NumberOfSections);
	if (SectionHeader == NULL) {
		perror(L"Unable to allocate section header\n");
		efi_status = EFI_OUT_OF_RESOURCES;
		goto done;
	}

	/*
	 * Validate section locations and sizes, and sort the table into
	 * our newly allocated header table
	 */
	SumOfSectionBytes = 0;
	Section = context->FirstSection;
	for (index = 0; index < context->NumberOfSections; index++) {
		EFI_IMAGE_SECTION_HEADER *SectionPtr;
		char *base;
		size_t size;

		efi_status = get_section_vma(index, data, datasize, context,
					     &base, &size, &SectionPtr);
		if (efi_status == EFI_NOT_FOUND)
			break;
		if (EFI_ERROR(efi_status)) {
			perror(L"Malformed section header\n");
			goto done;
		}

		/* Validate section size is within image. */
		if (SectionPtr->SizeOfRawData >
		    datasize - SumOfBytesHashed - SumOfSectionBytes) {
			perror(L"Malformed section %d size\n", index);
			efi_status = EFI_INVALID_PARAMETER;
			goto done;
		}
		SumOfSectionBytes += SectionPtr->SizeOfRawData;

		pos = index;
		while ((pos > 0) && (Section->PointerToRawData < SectionHeader[pos - 1].PointerToRawData)) {
			CopyMem (&SectionHeader[pos], &SectionHeader[pos - 1], sizeof (EFI_IMAGE_SECTION_HEADER));
			pos--;
		}
		CopyMem (&SectionHeader[pos], Section, sizeof (EFI_IMAGE_SECTION_HEADER));
		Section += 1;

	}

	/* Hash the sections */
	for (index = 0; index < context->NumberOfSections; index++) {
		Section = &SectionHeader[index];
		if (Section->SizeOfRawData == 0) {
			continue;
		}

		hashbase  = ImageAddress(data, datasize,
					 Section->PointerToRawData);
		if (!hashbase) {
			perror(L"Malformed section header\n");
			efi_status = EFI_INVALID_PARAMETER;
			goto done;
		}

		/* Verify hashsize within image. */
		if (Section->SizeOfRawData >
		    datasize - Section->PointerToRawData) {
			perror(L"Malformed section raw size %d\n", index);
			efi_status = EFI_INVALID_PARAMETER;
			goto done;
		}
		hashsize  = (unsigned int) Section->SizeOfRawData;
		check_size(data, datasize, hashbase, hashsize);

		if (!(Sha256Update(sha256ctx, hashbase, hashsize)) ||
		    !(Sha1Update(sha1ctx, hashbase, hashsize))) {
			perror(L"Unable to generate hash\n");
			efi_status = EFI_OUT_OF_RESOURCES;
			goto done;
		}
		SumOfBytesHashed += Section->SizeOfRawData;
	}

	/* Hash all remaining data up to SecDir if SecDir->Size is not 0 */
	if (datasize > SumOfBytesHashed && context->SecDir->Size) {
		hashbase = data + SumOfBytesHashed;
		hashsize = datasize - context->SecDir->Size - SumOfBytesHashed;

		if ((datasize - SumOfBytesHashed < context->SecDir->Size) ||
		    (SumOfBytesHashed + hashsize != context->SecDir->VirtualAddress)) {
			perror(L"Malformed binary after Attribute Certificate Table\n");
			console_print(L"datasize: %u SumOfBytesHashed: %u SecDir->Size: %lu\n",
				      datasize, SumOfBytesHashed, context->SecDir->Size);
			console_print(L"hashsize: %u SecDir->VirtualAddress: 0x%08lx\n",
				      hashsize, context->SecDir->VirtualAddress);
			efi_status = EFI_INVALID_PARAMETER;
			goto done;
		}
		check_size(data, datasize, hashbase, hashsize);

		if (!(Sha256Update(sha256ctx, hashbase, hashsize)) ||
		    !(Sha1Update(sha1ctx, hashbase, hashsize))) {
			perror(L"Unable to generate hash\n");
			efi_status = EFI_OUT_OF_RESOURCES;
			goto done;
		}

#if 1
	}
#else // we have to migrate to doing this later :/
		SumOfBytesHashed += hashsize;
	}