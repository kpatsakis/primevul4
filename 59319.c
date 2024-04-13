static bool get_nic_if_avail(int fd, struct alloted_s *names, int pid, char *intype, char *br, int allowed, char **nicname, char **cnic)
{
	off_t len, slen;
	struct stat sb;
	char *buf = NULL, *newline;
	int ret, count = 0;
	char *owner;
	struct alloted_s *n;

	for (n=names; n!=NULL; n=n->next)
		cull_entries(fd, n->name, intype, br);

	if (allowed == 0)
		return false;

	owner = names->name;

	if (fstat(fd, &sb) < 0) {
		fprintf(stderr, "Failed to fstat: %s\n", strerror(errno));
		return false;
	}
	len = sb.st_size;
	if (len != 0) {
		buf = mmap(NULL, len, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
		if (buf == MAP_FAILED) {
			fprintf(stderr, "Failed to create mapping\n");
			return false;
		}

		owner = NULL;
		for (n=names; n!=NULL; n=n->next) {
			count = count_entries(buf, len, n->name, intype, br);

			if (count >= n->allowed)
				continue;

			owner = n->name;
			break;
		}
	}

	if (owner == NULL)
		return false;

	if (!get_new_nicname(nicname, br, pid, cnic))
		return false;
	/* owner  ' ' intype ' ' br ' ' *nicname + '\n' + '\0' */
	slen = strlen(owner) + strlen(intype) + strlen(br) + strlen(*nicname) + 5;
	newline = alloca(slen);
	ret = snprintf(newline, slen, "%s %s %s %s\n", owner, intype, br, *nicname);
	if (ret < 0 || ret >= slen) {
		if (lxc_netdev_delete_by_name(*nicname) != 0)
			fprintf(stderr, "Error unlinking %s!\n", *nicname);
		return false;
	}
	if (len)
		munmap(buf, len);
	if (ftruncate(fd, len + slen))
		fprintf(stderr, "Failed to set new file size\n");
	buf = mmap(NULL, len + slen, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if (buf == MAP_FAILED) {
		fprintf(stderr, "Failed to create mapping after extending: %s\n", strerror(errno));
		if (lxc_netdev_delete_by_name(*nicname) != 0)
			fprintf(stderr, "Error unlinking %s!\n", *nicname);
		return false;
	}
	strcpy(buf+len, newline);
	munmap(buf, len+slen);
	return true;
}
