static ut64 get_import_addr(ELFOBJ *bin, int sym) {
	Elf_(Rel) *rel = NULL;
	Elf_(Rela) *rela = NULL;
	ut8 rl[sizeof (Elf_(Rel))] = {0};
	ut8 rla[sizeof (Elf_(Rela))] = {0};
	RBinElfSection *rel_sec = NULL;
	Elf_(Addr) plt_sym_addr = -1;
	ut64 got_addr, got_offset;
	ut64 plt_addr;
	int j, k, tsize, len, nrel;
	bool is_rela = false;
	const char *rel_sect[] = { ".rel.plt", ".rela.plt", ".rel.dyn", ".rela.dyn", NULL };
	const char *rela_sect[] = { ".rela.plt", ".rel.plt", ".rela.dyn", ".rel.dyn", NULL };

	if ((!bin->shdr || !bin->strtab) && !bin->phdr) {
		return -1;
	}
	if ((got_offset = Elf_(r_bin_elf_get_section_offset) (bin, ".got")) == -1 &&
		(got_offset = Elf_(r_bin_elf_get_section_offset) (bin, ".got.plt")) == -1) {
		return -1;
	}
	if ((got_addr = Elf_(r_bin_elf_get_section_addr) (bin, ".got")) == -1 &&
		(got_addr = Elf_(r_bin_elf_get_section_addr) (bin, ".got.plt")) == -1) {
		return -1;
	}
	if (bin->is_rela == DT_REL) {
		j = 0;
		while (!rel_sec && rel_sect[j]) {
			rel_sec = get_section_by_name (bin, rel_sect[j++]);
		}
		tsize = sizeof (Elf_(Rel));
	} else if (bin->is_rela == DT_RELA) {
		j = 0;
		while (!rel_sec && rela_sect[j]) {
			rel_sec = get_section_by_name (bin, rela_sect[j++]);
		}
		is_rela = true;
		tsize = sizeof (Elf_(Rela));
	}
	if (!rel_sec) {
		return -1;
	}
	if (rel_sec->size < 1) {
		return -1;
	}
	nrel = (ut32)((int)rel_sec->size / (int)tsize);
	if (nrel < 1) {
		return -1;
	}
	if (is_rela) {
		rela = calloc (nrel, tsize);
		if (!rela) {
			return -1;
		}
	} else {
		rel = calloc (nrel, tsize);
		if (!rel) {
			return -1;
		}
	}
	for (j = k = 0; j < rel_sec->size && k < nrel; j += tsize, k++) {
		int l = 0;
		if (rel_sec->offset + j > bin->size) {
			goto out;
		}
		if (rel_sec->offset + j + tsize > bin->size) {
			goto out;
		}
		len = r_buf_read_at (
			bin->b, rel_sec->offset + j, is_rela ? rla : rl,
			is_rela ? sizeof (Elf_ (Rela)) : sizeof (Elf_ (Rel)));
		if (len < 1) {
			goto out;
		}
#if R_BIN_ELF64
		if (is_rela) {
			rela[k].r_offset = READ64 (rla, l)
			rela[k].r_info = READ64 (rla, l)
			rela[k].r_addend = READ64 (rla, l)
		} else {
			rel[k].r_offset = READ64 (rl, l)
			rel[k].r_info = READ64 (rl, l)
		}
#else
		if (is_rela) {
			rela[k].r_offset = READ32 (rla, l)
			rela[k].r_info = READ32 (rla, l)
			rela[k].r_addend = READ32 (rla, l)
		} else {
			rel[k].r_offset = READ32 (rl, l)
			rel[k].r_info = READ32 (rl, l)
		}
#endif
		int reloc_type = ELF_R_TYPE (REL_TYPE);
		int reloc_sym = ELF_R_SYM (REL_TYPE);

		if (reloc_sym == sym) {
			int of = REL_OFFSET;
			of = of - got_addr + got_offset;
			switch (bin->ehdr.e_machine) {
			case EM_PPC:
			case EM_PPC64:
				{
					RBinElfSection *s = get_section_by_name (bin, ".plt");
					if (s) {
						ut8 buf[4];
						ut64 base;
						len = r_buf_read_at (bin->b, s->offset, buf, sizeof (buf));
						if (len < 4) {
							goto out;
						}
						base = r_read_be32 (buf);
						base -= (nrel * 16);
						base += (k * 16);
						plt_addr = base;
						free (REL);
						return plt_addr;
					}
				}
				break;
			case EM_SPARC:
			case EM_SPARCV9:
			case EM_SPARC32PLUS:
				plt_addr = Elf_(r_bin_elf_get_section_addr) (bin, ".plt");
				if (plt_addr == -1) {
					free (rela);
					return -1;
				}
				if (reloc_type == R_386_PC16) {
					plt_addr += k * 12 + 20;
					if (plt_addr & 1) {
						plt_addr--;
					}
					free (REL);
					return plt_addr;
				} else {
					bprintf ("Unknown sparc reloc type %d\n", reloc_type);
				}
				/* SPARC */
				break;
			case EM_ARM:
			case EM_AARCH64:
				plt_addr = Elf_(r_bin_elf_get_section_addr) (bin, ".plt");
				if (plt_addr == -1) {
					free (rela);
					return UT32_MAX;
				}
				switch (reloc_type) {
				case R_386_8:
					{
						plt_addr += k * 12 + 20;
						if (plt_addr & 1) {
							plt_addr--;
						}
						free (REL);
						return plt_addr;
					}
					break;
				case 1026: // arm64 aarch64
					plt_sym_addr = plt_addr + k * 16 + 32;
					goto done;
				default:
					bprintf ("Unsupported relocation type for imports %d\n", reloc_type);
					break;
				}
				break;
			case EM_386:
			case EM_X86_64:
				switch (reloc_type) {
				case 1: // unknown relocs found in voidlinux for x86-64
				case R_386_GLOB_DAT:
				case R_386_JMP_SLOT:
					{
					ut8 buf[8];
					if (of + sizeof(Elf_(Addr)) < bin->size) {
						if (of > bin->size || of + sizeof (Elf_(Addr)) > bin->size) {
							goto out;
						}
						len = r_buf_read_at (bin->b, of, buf, sizeof (Elf_(Addr)));
						if (len < -1) {
							goto out;
						}
						plt_sym_addr = sizeof (Elf_(Addr)) == 4
									 ? r_read_le32 (buf)
									 : r_read_le64 (buf);

						if (!plt_sym_addr) {
							RBinElfSection *s = get_section_by_name (bin, ".plt.got");
 							if (Elf_(r_bin_elf_has_relro)(bin) < R_ELF_PART_RELRO || !s) {
								goto done;
							}
							plt_addr = s->offset;
							of = of + got_addr - got_offset;
							while (plt_addr + 2 + 4 < s->offset + s->size) {
								/*we try to locate the plt entry that correspond with the relocation
								  since got does not point back to .plt. In this case it has the following 
								  form

								  ff253a152000   JMP QWORD [RIP + 0x20153A]
								  6690		     NOP
								  ----
								  ff25ec9f0408   JMP DWORD [reloc.puts_236]

								  plt_addr + 2 to remove jmp opcode and get the imm reading 4
								  and if RIP (plt_addr + 6) + imm == rel->offset 
								  return plt_addr, that will be our sym addr

								  perhaps this hack doesn't work on 32 bits
								 */
								len = r_buf_read_at (bin->b, plt_addr + 2, buf, 4);
								if (len < -1) {
									goto out;
								}
								plt_sym_addr = sizeof (Elf_(Addr)) == 4
										? r_read_le32 (buf)
										: r_read_le64 (buf);

								if ((plt_addr + 6 + Elf_(r_bin_elf_v2p) (bin, plt_sym_addr)) == of) {
									plt_sym_addr = plt_addr;
									goto done;
								} else if (plt_sym_addr == of) {
									plt_sym_addr = plt_addr;
									goto done;
								}
								plt_addr += 8;
							}
						} else {
							plt_sym_addr -= 6;
						}
						goto done;
					}
					break;
					}
				default:
					bprintf ("Unsupported relocation type for imports %d\n", reloc_type);
					free (REL);
					return of;
					break;
				}
				break;
			case 8:
				{
					RBinElfSection *s = get_section_by_name (bin, ".rela.plt");
					if (s) {
						ut8 buf[1024];
						const ut8 *base;
						plt_addr = s->rva + s->size;
						len = r_buf_read_at (bin->b, s->offset + s->size, buf, sizeof (buf));
						if (len != sizeof (buf)) {
						}
						base = r_mem_mem_aligned (buf, sizeof (buf), (const ut8*)"\x3c\x0f\x00", 3, 4);
						if (base) {
							plt_addr += (int)(size_t)(base - buf);
						} else {
							plt_addr += 108 + 8; // HARDCODED HACK
						}
						plt_addr += k * 16;
						free (REL);
						return plt_addr;
					}
				}
				break;
			default:
				bprintf ("Unsupported relocs type %d for arch %d\n",
					reloc_type, bin->ehdr.e_machine);
				break;
			}
		}
	}
done:
	free (REL);
	return plt_sym_addr;
out:
	free (REL);
	return -1;
}
