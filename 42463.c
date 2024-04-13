static void entry_status(Node *e, char *page)
{
	char *dp;
	char *status = "disabled";
	const char * flags = "flags: ";

	if (test_bit(Enabled, &e->flags))
		status = "enabled";

	if (!VERBOSE_STATUS) {
		sprintf(page, "%s\n", status);
		return;
	}

	sprintf(page, "%s\ninterpreter %s\n", status, e->interpreter);
	dp = page + strlen(page);

	/* print the special flags */
	sprintf (dp, "%s", flags);
	dp += strlen (flags);
	if (e->flags & MISC_FMT_PRESERVE_ARGV0) {
		*dp ++ = 'P';
	}
	if (e->flags & MISC_FMT_OPEN_BINARY) {
		*dp ++ = 'O';
	}
	if (e->flags & MISC_FMT_CREDENTIALS) {
		*dp ++ = 'C';
	}
	*dp ++ = '\n';


	if (!test_bit(Magic, &e->flags)) {
		sprintf(dp, "extension .%s\n", e->magic);
	} else {
		int i;

		sprintf(dp, "offset %i\nmagic ", e->offset);
		dp = page + strlen(page);
		for (i = 0; i < e->size; i++) {
			sprintf(dp, "%02x", 0xff & (int) (e->magic[i]));
			dp += 2;
		}
		if (e->mask) {
			sprintf(dp, "\nmask ");
			dp += 6;
			for (i = 0; i < e->size; i++) {
				sprintf(dp, "%02x", 0xff & (int) (e->mask[i]));
				dp += 2;
			}
		}
		*dp++ = '\n';
		*dp = '\0';
	}
}
