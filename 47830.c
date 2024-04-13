static inline int arch_check_elf(struct elfhdr *ehdr, bool has_interp,
				 struct arch_elf_state *state)
{
	/* Dummy implementation, always proceed */
	return 0;
}
