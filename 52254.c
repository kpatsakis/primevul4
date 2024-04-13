archive_read_format_7zip_read_header(struct archive_read *a,
	struct archive_entry *entry)
{
	struct _7zip *zip = (struct _7zip *)a->format->data;
	struct _7zip_entry *zip_entry;
	int r, ret = ARCHIVE_OK;
	struct _7z_folder *folder = 0;
	uint64_t fidx = 0;

	/*
	 * It should be sufficient to call archive_read_next_header() for
	 * a reader to determine if an entry is encrypted or not. If the
	 * encryption of an entry is only detectable when calling
	 * archive_read_data(), so be it. We'll do the same check there
	 * as well.
	 */
	if (zip->has_encrypted_entries == ARCHIVE_READ_FORMAT_ENCRYPTION_DONT_KNOW) {
		zip->has_encrypted_entries = 0;
	}

	a->archive.archive_format = ARCHIVE_FORMAT_7ZIP;
	if (a->archive.archive_format_name == NULL)
		a->archive.archive_format_name = "7-Zip";

	if (zip->entries == NULL) {
		struct _7z_header_info header;

		memset(&header, 0, sizeof(header));
		r = slurp_central_directory(a, zip, &header);
		free_Header(&header);
		if (r != ARCHIVE_OK)
			return (r);
		zip->entries_remaining = (size_t)zip->numFiles;
		zip->entry = zip->entries;
	} else {
		++zip->entry;
	}
	zip_entry = zip->entry;

	if (zip->entries_remaining <= 0 || zip_entry == NULL)
		return ARCHIVE_EOF;
	--zip->entries_remaining;

	zip->entry_offset = 0;
	zip->end_of_entry = 0;
	zip->entry_crc32 = crc32(0, NULL, 0);

	/* Setup a string conversion for a filename. */
	if (zip->sconv == NULL) {
		zip->sconv = archive_string_conversion_from_charset(
		    &a->archive, "UTF-16LE", 1);
		if (zip->sconv == NULL)
			return (ARCHIVE_FATAL);
	}

	/* Figure out if the entry is encrypted by looking at the folder
	   that is associated to the current 7zip entry. If the folder
	   has a coder with a _7Z_CRYPTO codec then the folder is encrypted.
	   Hence the entry must also be encrypted. */
	if (zip_entry && zip_entry->folderIndex < zip->si.ci.numFolders) {
		folder = &(zip->si.ci.folders[zip_entry->folderIndex]);
		for (fidx=0; folder && fidx<folder->numCoders; fidx++) {
			switch(folder->coders[fidx].codec) {
				case _7Z_CRYPTO_MAIN_ZIP:
				case _7Z_CRYPTO_RAR_29:
				case _7Z_CRYPTO_AES_256_SHA_256: {
					archive_entry_set_is_data_encrypted(entry, 1);
					zip->has_encrypted_entries = 1;
					break;
				}
			}
		}
	}

	/* Now that we've checked for encryption, if there were still no
	 * encrypted entries found we can say for sure that there are none.
	 */
	if (zip->has_encrypted_entries == ARCHIVE_READ_FORMAT_ENCRYPTION_DONT_KNOW) {
		zip->has_encrypted_entries = 0;
	}

	if (archive_entry_copy_pathname_l(entry,
	    (const char *)zip_entry->utf16name,
	    zip_entry->name_len, zip->sconv) != 0) {
		if (errno == ENOMEM) {
			archive_set_error(&a->archive, ENOMEM,
			    "Can't allocate memory for Pathname");
			return (ARCHIVE_FATAL);
		}
		archive_set_error(&a->archive,
		    ARCHIVE_ERRNO_FILE_FORMAT,
		    "Pathname cannot be converted "
		    "from %s to current locale.",
		    archive_string_conversion_charset_name(zip->sconv));
		ret = ARCHIVE_WARN;
	}

	/* Populate some additional entry fields: */
	archive_entry_set_mode(entry, zip_entry->mode);
	if (zip_entry->flg & MTIME_IS_SET)
		archive_entry_set_mtime(entry, zip_entry->mtime,
			zip_entry->mtime_ns);
	if (zip_entry->flg & CTIME_IS_SET)
		archive_entry_set_ctime(entry, zip_entry->ctime,
		    zip_entry->ctime_ns);
	if (zip_entry->flg & ATIME_IS_SET)
		archive_entry_set_atime(entry, zip_entry->atime,
		    zip_entry->atime_ns);
	if (zip_entry->ssIndex != (uint32_t)-1) {
		zip->entry_bytes_remaining =
		    zip->si.ss.unpackSizes[zip_entry->ssIndex];
		archive_entry_set_size(entry, zip->entry_bytes_remaining);
	} else {
		zip->entry_bytes_remaining = 0;
		archive_entry_set_size(entry, 0);
	}

	/* If there's no body, force read_data() to return EOF immediately. */
	if (zip->entry_bytes_remaining < 1)
		zip->end_of_entry = 1;

	if ((zip_entry->mode & AE_IFMT) == AE_IFLNK) {
		unsigned char *symname = NULL;
		size_t symsize = 0;

		/*
		 * Symbolic-name is recorded as its contents. We have to
		 * read the contents at this time.
		 */
		while (zip->entry_bytes_remaining > 0) {
			const void *buff;
			unsigned char *mem;
			size_t size;
			int64_t offset;

			r = archive_read_format_7zip_read_data(a, &buff,
				&size, &offset);
			if (r < ARCHIVE_WARN) {
				free(symname);
				return (r);
			}
			mem = realloc(symname, symsize + size + 1);
			if (mem == NULL) {
				free(symname);
				archive_set_error(&a->archive, ENOMEM,
				    "Can't allocate memory for Symname");
				return (ARCHIVE_FATAL);
			}
			symname = mem;
			memcpy(symname+symsize, buff, size);
			symsize += size;
		}
		if (symsize == 0) {
			/* If there is no synname, handle it as a regular
			 * file. */
			zip_entry->mode &= ~AE_IFMT;
			zip_entry->mode |= AE_IFREG;
			archive_entry_set_mode(entry, zip_entry->mode);
		} else {
			symname[symsize] = '\0';
			archive_entry_copy_symlink(entry,
			    (const char *)symname);
		}
		free(symname);
		archive_entry_set_size(entry, 0);
	}

	/* Set up a more descriptive format name. */
	sprintf(zip->format_name, "7-Zip");
	a->archive.archive_format_name = zip->format_name;

	return (ret);
}
