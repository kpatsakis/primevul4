check_fmt(struct magic_set *ms, struct magic *m)
{
	file_regex_t rx;
	int rc, rv = -1;

	if (strchr(m->desc, '%') == NULL)
		return 0;

	rc = file_regcomp(&rx, "%[-0-9\\.]*s", REG_EXTENDED|REG_NOSUB);
	if (rc) {
		file_regerror(&rx, rc, ms);
	} else {
		rc = file_regexec(&rx, m->desc, 0, 0, 0);
		rv = !rc;
	}
	file_regfree(&rx);
	return rv;
}
