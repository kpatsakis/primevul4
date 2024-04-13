static bool cull_entries(int fd, char *me, char *t, char *br)
{
	struct stat sb;
	char *buf, *p, *e, *nic;
	off_t len;
	struct entry_line *entry_lines = NULL;
	int i, n = 0;

	nic = alloca(100);

	if (fstat(fd, &sb) < 0) {
		fprintf(stderr, "Failed to fstat: %s\n", strerror(errno));
		return false;
	}
	len = sb.st_size;
	if (len == 0)
		return true;
	buf = mmap(NULL, len, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if (buf == MAP_FAILED) {
		fprintf(stderr, "Failed to create mapping: %s\n", strerror(errno));
		return false;
	}

	p = buf;
	e = buf + len;
	while ((p = find_line(p, e, me, t, br)) != NULL) {
		struct entry_line *newe = realloc(entry_lines, sizeof(*entry_lines)*(n+1));
		if (!newe) {
			free(entry_lines);
			return false;
		}
		entry_lines = newe;
		entry_lines[n].start = p;
		entry_lines[n].len = get_eol(p, e) - entry_lines[n].start;
		entry_lines[n].keep = true;
		n++;
		if (!get_nic_from_line(p, &nic))
			continue;
		if (nic && !nic_exists(nic))
			entry_lines[n-1].keep = false;
		p += entry_lines[n-1].len + 1;
		if (p >= e)
			break;
 	}
	p = buf;
	for (i=0; i<n; i++) {
		if (!entry_lines[i].keep)
			continue;
		memcpy(p, entry_lines[i].start, entry_lines[i].len);
		p += entry_lines[i].len;
		*p = '\n';
		p++;
	}
	free(entry_lines);
	munmap(buf, sb.st_size);
	if (ftruncate(fd, p-buf))
		fprintf(stderr, "Failed to set new file size\n");
	return true;
}
