do_pax_note(struct magic_set *ms, unsigned char *nbuf, uint32_t type,
    int swap, uint32_t namesz, uint32_t descsz,
    size_t noff, size_t doff, int *flags)
{
	if (namesz == 4 && strcmp((char *)&nbuf[noff], "PaX") == 0 &&
	    type == NT_NETBSD_PAX && descsz == 4) {
		static const char *pax[] = {
		    "+mprotect",
		    "-mprotect",
		    "+segvguard",
		    "-segvguard",
		    "+ASLR",
		    "-ASLR",
		};
		uint32_t desc;
		size_t i;
		int did = 0;

		*flags |= FLAGS_DID_NETBSD_PAX;
		(void)memcpy(&desc, &nbuf[doff], sizeof(desc));
		desc = elf_getu32(swap, desc);

		if (desc && file_printf(ms, ", PaX: ") == -1)
			return 1;

		for (i = 0; i < __arraycount(pax); i++) {
			if (((1 << i) & desc) == 0)
				continue;
			if (file_printf(ms, "%s%s", did++ ? "," : "",
			    pax[i]) == -1)
				return 1;
		}
		return 1;
	}
	return 0;
}
