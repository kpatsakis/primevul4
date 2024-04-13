archive_read_format_iso9660_options(struct archive_read *a,
		const char *key, const char *val)
{
	struct iso9660 *iso9660;

	iso9660 = (struct iso9660 *)(a->format->data);

	if (strcmp(key, "joliet") == 0) {
		if (val == NULL || strcmp(val, "off") == 0 ||
				strcmp(val, "ignore") == 0 ||
				strcmp(val, "disable") == 0 ||
				strcmp(val, "0") == 0)
			iso9660->opt_support_joliet = 0;
		else
			iso9660->opt_support_joliet = 1;
		return (ARCHIVE_OK);
	}
	if (strcmp(key, "rockridge") == 0 ||
	    strcmp(key, "Rockridge") == 0) {
		iso9660->opt_support_rockridge = val != NULL;
		return (ARCHIVE_OK);
	}

	/* Note: The "warn" return is just to inform the options
	 * supervisor that we didn't handle it.  It will generate
	 * a suitable error if no one used this option. */
	return (ARCHIVE_WARN);
}
