do_note_freebsd_version(struct magic_set *ms, int swap, void *v)
{
	uint32_t desc;

	(void)memcpy(&desc, v, sizeof(desc));
	desc = elf_getu32(swap, desc);
	if (file_printf(ms, ", for FreeBSD") == -1)
		return;

	/*
	 * Contents is __FreeBSD_version, whose relation to OS
	 * versions is defined by a huge table in the Porter's
	 * Handbook.  This is the general scheme:
	 * 
	 * Releases:
	 * 	Mmp000 (before 4.10)
	 * 	Mmi0p0 (before 5.0)
	 * 	Mmm0p0
	 * 
	 * Development branches:
	 * 	Mmpxxx (before 4.6)
	 * 	Mmp1xx (before 4.10)
	 * 	Mmi1xx (before 5.0)
	 * 	M000xx (pre-M.0)
	 * 	Mmm1xx
	 * 
	 * M = major version
	 * m = minor version
	 * i = minor version increment (491000 -> 4.10)
	 * p = patchlevel
	 * x = revision
	 * 
	 * The first release of FreeBSD to use ELF by default
	 * was version 3.0.
	 */
	if (desc == 460002) {
		if (file_printf(ms, " 4.6.2") == -1)
			return;
	} else if (desc < 460100) {
		if (file_printf(ms, " %d.%d", desc / 100000,
		    desc / 10000 % 10) == -1)
			return;
		if (desc / 1000 % 10 > 0)
			if (file_printf(ms, ".%d", desc / 1000 % 10) == -1)
				return;
		if ((desc % 1000 > 0) || (desc % 100000 == 0))
			if (file_printf(ms, " (%d)", desc) == -1)
				return;
	} else if (desc < 500000) {
		if (file_printf(ms, " %d.%d", desc / 100000,
		    desc / 10000 % 10 + desc / 1000 % 10) == -1)
			return;
		if (desc / 100 % 10 > 0) {
			if (file_printf(ms, " (%d)", desc) == -1)
				return;
		} else if (desc / 10 % 10 > 0) {
			if (file_printf(ms, ".%d", desc / 10 % 10) == -1)
				return;
		}
	} else {
		if (file_printf(ms, " %d.%d", desc / 100000,
		    desc / 1000 % 100) == -1)
			return;
		if ((desc / 100 % 10 > 0) ||
		    (desc % 100000 / 100 == 0)) {
			if (file_printf(ms, " (%d)", desc) == -1)
				return;
		} else if (desc / 10 % 10 > 0) {
			if (file_printf(ms, ".%d", desc / 10 % 10) == -1)
				return;
		}
	}
}
