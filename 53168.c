archive_read_format_zip_options(struct archive_read *a,
    const char *key, const char *val)
{
	struct zip *zip;
	int ret = ARCHIVE_FAILED;

	zip = (struct zip *)(a->format->data);
	if (strcmp(key, "compat-2x")  == 0) {
		/* Handle filenames as libarchive 2.x */
		zip->init_default_conversion = (val != NULL) ? 1 : 0;
		return (ARCHIVE_OK);
	} else if (strcmp(key, "hdrcharset")  == 0) {
		if (val == NULL || val[0] == 0)
			archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
			    "zip: hdrcharset option needs a character-set name"
			);
		else {
			zip->sconv = archive_string_conversion_from_charset(
			    &a->archive, val, 0);
			if (zip->sconv != NULL) {
				if (strcmp(val, "UTF-8") == 0)
					zip->sconv_utf8 = zip->sconv;
				ret = ARCHIVE_OK;
			} else
				ret = ARCHIVE_FATAL;
		}
		return (ret);
	} else if (strcmp(key, "ignorecrc32") == 0) {
		/* Mostly useful for testing. */
		if (val == NULL || val[0] == 0) {
			zip->crc32func = real_crc32;
			zip->ignore_crc32 = 0;
		} else {
			zip->crc32func = fake_crc32;
			zip->ignore_crc32 = 1;
		}
		return (ARCHIVE_OK);
	} else if (strcmp(key, "mac-ext") == 0) {
		zip->process_mac_extensions = (val != NULL && val[0] != 0);
		return (ARCHIVE_OK);
	}

	/* Note: The "warn" return is just to inform the options
	 * supervisor that we didn't handle it.  It will generate
	 * a suitable error if no one used this option. */
	return (ARCHIVE_WARN);
}
