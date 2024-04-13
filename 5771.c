relocate_coff (PE_COFF_LOADER_IMAGE_CONTEXT *context,
	       EFI_IMAGE_SECTION_HEADER *Section,
	       void *orig, void *data)
{
	EFI_IMAGE_BASE_RELOCATION *RelocBase, *RelocBaseEnd;
	UINT64 Adjust;
	UINT16 *Reloc, *RelocEnd;
	char *Fixup, *FixupBase;
	UINT16 *Fixup16;
	UINT32 *Fixup32;
	UINT64 *Fixup64;
	int size = context->ImageSize;
	void *ImageEnd = (char *)orig + size;
	int n = 0;

	/* Alright, so here's how this works:
	 *
	 * context->RelocDir gives us two things:
	 * - the VA the table of base relocation blocks are (maybe) to be
	 *   mapped at (RelocDir->VirtualAddress)
	 * - the virtual size (RelocDir->Size)
	 *
	 * The .reloc section (Section here) gives us some other things:
	 * - the name! kind of. (Section->Name)
	 * - the virtual size (Section->VirtualSize), which should be the same
	 *   as RelocDir->Size
	 * - the virtual address (Section->VirtualAddress)
	 * - the file section size (Section->SizeOfRawData), which is
	 *   a multiple of OptHdr->FileAlignment.  Only useful for image
	 *   validation, not really useful for iteration bounds.
	 * - the file address (Section->PointerToRawData)
	 * - a bunch of stuff we don't use that's 0 in our binaries usually
	 * - Flags (Section->Characteristics)
	 *
	 * and then the thing that's actually at the file address is an array
	 * of EFI_IMAGE_BASE_RELOCATION structs with some values packed behind
	 * them.  The SizeOfBlock field of this structure includes the
	 * structure itself, and adding it to that structure's address will
	 * yield the next entry in the array.
	 */
	RelocBase = ImageAddress(orig, size, Section->PointerToRawData);
	/* RelocBaseEnd here is the address of the first entry /past/ the
	 * table.  */
	RelocBaseEnd = ImageAddress(orig, size, Section->PointerToRawData +
						Section->Misc.VirtualSize);

	if (!RelocBase && !RelocBaseEnd)
		return EFI_SUCCESS;

	if (!RelocBase || !RelocBaseEnd) {
		perror(L"Reloc table overflows binary\n");
		return EFI_UNSUPPORTED;
	}

	Adjust = (UINTN)data - context->ImageAddress;

	if (Adjust == 0)
		return EFI_SUCCESS;

	while (RelocBase < RelocBaseEnd) {
		Reloc = (UINT16 *) ((char *) RelocBase + sizeof (EFI_IMAGE_BASE_RELOCATION));

		if (RelocBase->SizeOfBlock == 0) {
			perror(L"Reloc %d block size 0 is invalid\n", n);
			return EFI_UNSUPPORTED;
		} else if (RelocBase->SizeOfBlock > context->RelocDir->Size) {
			perror(L"Reloc %d block size %d greater than reloc dir"
					"size %d, which is invalid\n", n,
					RelocBase->SizeOfBlock,
					context->RelocDir->Size);
			return EFI_UNSUPPORTED;
		}

		RelocEnd = (UINT16 *) ((char *) RelocBase + RelocBase->SizeOfBlock);
		if ((void *)RelocEnd < orig || (void *)RelocEnd > ImageEnd) {
			perror(L"Reloc %d entry overflows binary\n", n);
			return EFI_UNSUPPORTED;
		}

		FixupBase = ImageAddress(data, size, RelocBase->VirtualAddress);
		if (!FixupBase) {
			perror(L"Reloc %d Invalid fixupbase\n", n);
			return EFI_UNSUPPORTED;
		}

		while (Reloc < RelocEnd) {
			Fixup = FixupBase + (*Reloc & 0xFFF);
			switch ((*Reloc) >> 12) {
			case EFI_IMAGE_REL_BASED_ABSOLUTE:
				break;

			case EFI_IMAGE_REL_BASED_HIGH:
				Fixup16   = (UINT16 *) Fixup;
				*Fixup16 = (UINT16) (*Fixup16 + ((UINT16) ((UINT32) Adjust >> 16)));
				break;

			case EFI_IMAGE_REL_BASED_LOW:
				Fixup16   = (UINT16 *) Fixup;
				*Fixup16  = (UINT16) (*Fixup16 + (UINT16) Adjust);
				break;

			case EFI_IMAGE_REL_BASED_HIGHLOW:
				Fixup32   = (UINT32 *) Fixup;
				*Fixup32  = *Fixup32 + (UINT32) Adjust;
				break;

			case EFI_IMAGE_REL_BASED_DIR64:
				Fixup64 = (UINT64 *) Fixup;
				*Fixup64 = *Fixup64 + (UINT64) Adjust;
				break;

			default:
				perror(L"Reloc %d Unknown relocation\n", n);
				return EFI_UNSUPPORTED;
			}
			Reloc += 1;
		}
		RelocBase = (EFI_IMAGE_BASE_RELOCATION *) RelocEnd;
		n++;
	}

	return EFI_SUCCESS;
}