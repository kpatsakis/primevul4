get_string_on_virtaddr(struct magic_set *ms,
    int swap, int clazz, int fd, off_t ph_off, int ph_num,
    off_t fsize, uint64_t virtaddr, char *buf, ssize_t buflen)
{
	char *bptr;
	off_t offset;

	if (buflen == 0)
		return 0;

	offset = get_offset_from_virtaddr(ms, swap, clazz, fd, ph_off, ph_num,
	    fsize, virtaddr);
	if ((buflen = pread(fd, buf, CAST(size_t, buflen), offset)) <= 0) {
		file_badread(ms);
		return 0;
	}

	buf[buflen - 1] = '\0';

	/* We expect only printable characters, so return if buffer contains
	 * non-printable character before the '\0' or just '\0'. */
	for (bptr = buf; *bptr && isprint((unsigned char)*bptr); bptr++)
		continue;
	if (*bptr != '\0')
		return 0;

	return bptr - buf;
}
