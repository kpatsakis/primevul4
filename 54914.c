static int read_reloc(ELFOBJ *bin, RBinElfReloc *r, int is_rela, ut64 offset) {
	ut8 *buf = bin->b->buf;
	int j = 0;
	if (offset + sizeof (Elf_ (Rela)) >
		    bin->size || offset + sizeof (Elf_(Rela)) < offset) {
		return -1;
	}
	if (is_rela == DT_RELA) {
		Elf_(Rela) rela;
#if R_BIN_ELF64
		rela.r_offset = READ64 (buf + offset, j)
		rela.r_info = READ64 (buf + offset, j)
		rela.r_addend = READ64 (buf + offset, j)
#else
		rela.r_offset = READ32 (buf + offset, j)
		rela.r_info = READ32 (buf + offset, j)
		rela.r_addend = READ32 (buf + offset, j)
#endif
		r->is_rela = is_rela;
		r->offset = rela.r_offset;
		r->type = ELF_R_TYPE (rela.r_info);
		r->sym = ELF_R_SYM (rela.r_info);
		r->last = 0;
		r->addend = rela.r_addend;
		return sizeof (Elf_(Rela));
	} else {
		Elf_(Rel) rel;
#if R_BIN_ELF64
		rel.r_offset = READ64 (buf + offset, j)
		rel.r_info = READ64 (buf + offset, j)
#else
		rel.r_offset = READ32 (buf + offset, j)
		rel.r_info = READ32 (buf + offset, j)
#endif
		r->is_rela = is_rela;
		r->offset = rel.r_offset;
		r->type = ELF_R_TYPE (rel.r_info);
		r->sym = ELF_R_SYM (rel.r_info);
		r->last = 0;
		return sizeof (Elf_(Rel));
	}
}
