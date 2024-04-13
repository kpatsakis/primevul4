do_auxv_note(struct magic_set *ms, unsigned char *nbuf, uint32_t type,
    int swap, uint32_t namesz __attribute__((__unused__)),
    uint32_t descsz __attribute__((__unused__)),
    size_t noff __attribute__((__unused__)), size_t doff,
    int *flags, size_t size __attribute__((__unused__)), int clazz,
    int fd, off_t ph_off, int ph_num, off_t fsize)
{
#ifdef ELFCORE
	Aux32Info auxv32;
	Aux64Info auxv64;
	size_t elsize = xauxv_sizeof;
	const char *tag;
	int is_string;
	size_t nval;

	if ((*flags & (FLAGS_IS_CORE|FLAGS_DID_CORE_STYLE)) !=
	    (FLAGS_IS_CORE|FLAGS_DID_CORE_STYLE))
		return 0;

	switch (*flags & FLAGS_CORE_STYLE) {
	case OS_STYLE_SVR4:
		if (type != NT_AUXV)
			return 0;
		break;
#ifdef notyet
	case OS_STYLE_NETBSD:
		if (type != NT_NETBSD_CORE_AUXV)
			return 0;
		break;
	case OS_STYLE_FREEBSD:
		if (type != NT_FREEBSD_PROCSTAT_AUXV)
			return 0;
		break;
#endif
	default:
		return 0;
	}

	*flags |= FLAGS_DID_AUXV;

	nval = 0;
	for (size_t off = 0; off + elsize <= descsz; off += elsize) {
		(void)memcpy(xauxv_addr, &nbuf[doff + off], xauxv_sizeof);
		/* Limit processing to 50 vector entries to prevent DoS */
		if (nval++ >= 50) {
			file_error(ms, 0, "Too many ELF Auxv elements");
			return 1;
		}

		switch(xauxv_type) {
		case AT_LINUX_EXECFN:
			is_string = 1;
			tag = "execfn";
			break;
		case AT_LINUX_PLATFORM:
			is_string = 1;
			tag = "platform";
			break;
		case AT_LINUX_UID:
			is_string = 0;
			tag = "real uid";
			break;
		case AT_LINUX_GID:
			is_string = 0;
			tag = "real gid";
			break;
		case AT_LINUX_EUID:
			is_string = 0;
			tag = "effective uid";
			break;
		case AT_LINUX_EGID:
			is_string = 0;
			tag = "effective gid";
			break;
		default:
			is_string = 0;
			tag = NULL;
			break;
		}

		if (tag == NULL)
			continue;

		if (is_string) {
			char buf[256];
			ssize_t buflen;
			buflen = get_string_on_virtaddr(ms, swap, clazz, fd,
			    ph_off, ph_num, fsize, xauxv_val, buf, sizeof(buf));

			if (buflen == 0)
				continue;
			
			if (file_printf(ms, ", %s: '%s'", tag, buf) == -1)
				return 0;
		} else {
			if (file_printf(ms, ", %s: %d", tag, (int) xauxv_val)
			    == -1)
				return 0;
		}
	}
	return 1;
#else
	return 0;
#endif
}
