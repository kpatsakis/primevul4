do_note_netbsd_version(struct magic_set *ms, int swap, void *v)
{
	uint32_t desc;
	(void)memcpy(&desc, v, sizeof(desc));
	desc = elf_getu32(swap, desc);

	if (file_printf(ms, ", for NetBSD") == -1)
		return;
	/*
	 * The version number used to be stuck as 199905, and was thus
	 * basically content-free.  Newer versions of NetBSD have fixed
	 * this and now use the encoding of __NetBSD_Version__:
	 *
	 *	MMmmrrpp00
	 *
	 * M = major version
	 * m = minor version
	 * r = release ["",A-Z,Z[A-Z] but numeric]
	 * p = patchlevel
	 */
	if (desc > 100000000U) {
		uint32_t ver_patch = (desc / 100) % 100;
		uint32_t ver_rel = (desc / 10000) % 100;
		uint32_t ver_min = (desc / 1000000) % 100;
		uint32_t ver_maj = desc / 100000000;

		if (file_printf(ms, " %u.%u", ver_maj, ver_min) == -1)
			return;
		if (ver_rel == 0 && ver_patch != 0) {
			if (file_printf(ms, ".%u", ver_patch) == -1)
				return;
		} else if (ver_rel != 0) {
			while (ver_rel > 26) {
				if (file_printf(ms, "Z") == -1)
					return;
				ver_rel -= 26;
			}
			if (file_printf(ms, "%c", 'A' + ver_rel - 1)
			    == -1)
				return;
		}
	}
}
