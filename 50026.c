tar_init_info (GsfInfileTar *tar)
{
	TarHeader end;
	const TarHeader *header;
	gsf_off_t pos0 = gsf_input_tell (tar->source);
	char *pending_longname = NULL;

	memset (&end, 0, sizeof (end));

	while (tar->err == NULL &&
	       (header = (const TarHeader *)gsf_input_read (tar->source,
							    HEADER_SIZE,
							    NULL))) {
		char *name;
		gsf_off_t length;
		gsf_off_t offset;
		gint64 mtime;

		if (memcmp (header->filler, end.filler, sizeof (end.filler))) {
			tar->err = g_error_new (gsf_input_error_id (), 0,
						_("Invalid tar header"));
			break;
		}

		if (memcmp (header, &end, HEADER_SIZE) == 0)
			break;

		if (pending_longname) {
			name = pending_longname;
			pending_longname = NULL;
		} else
			name = g_strndup (header->name, sizeof (header->name));
		length = unpack_octal (tar, header->size, sizeof (header->size));
		offset = gsf_input_tell (tar->source);

		mtime = unpack_octal (tar, header->mtime, sizeof (header->mtime));

		switch (header->typeflag) {
		case '0': case 0: {
			/* Regular file. */
			GsfInfileTar *dir;
			const char *n = name, *s;
			TarChild c;

			/* This is deliberately slash-only.  */
			while ((s = strchr (n, '/')))
				n = s + 1;
			c.name = g_strdup (n);
			c.modtime = mtime > 0
				? g_date_time_new_from_unix_utc (mtime)
				: NULL;
			c.offset = offset;
			c.length = length;
			c.dir = NULL;
			dir = tar_directory_for_file (tar, name, FALSE);
			g_array_append_val (dir->children, c);
			break;
		}
		case '5': {
			/* Directory */
			(void)tar_directory_for_file (tar, name, TRUE);
			break;
		}
		case 'L': {
			const char *n;

			if (pending_longname ||
			    strcmp (name, MAGIC_LONGNAME) != 0) {
				tar->err = g_error_new (gsf_input_error_id (), 0,
							_("Invalid longname header"));
				break;
			}

			n = gsf_input_read (tar->source, length, NULL);
			if (!n) {
				tar->err = g_error_new (gsf_input_error_id (), 0,
							_("Failed to read longname"));
				break;
			}

			pending_longname = g_strndup (n, length);
			break;
		}
		default:
			/* Other -- ignore */
			break;
		}

		g_free (name);

		/* Round up to block size */
		length = (length + (BLOCK_SIZE - 1)) / BLOCK_SIZE * BLOCK_SIZE;

		if (!tar->err &&
		    gsf_input_seek (tar->source, offset + length, G_SEEK_SET)) {
			tar->err = g_error_new (gsf_input_error_id (), 0,
						_("Seek failed"));
			break;
		}
	}

	if (pending_longname) {
		if (!tar->err)
			tar->err = g_error_new (gsf_input_error_id (), 0,
						_("Truncated archive"));
		g_free (pending_longname);
	}

	if (tar->err)
		gsf_input_seek (tar->source, pos0, G_SEEK_SET);
}
