__archive_read_register_format(struct archive_read *a,
    void *format_data,
    const char *name,
    int (*bid)(struct archive_read *, int),
    int (*options)(struct archive_read *, const char *, const char *),
    int (*read_header)(struct archive_read *, struct archive_entry *),
    int (*read_data)(struct archive_read *, const void **, size_t *, int64_t *),
    int (*read_data_skip)(struct archive_read *),
    int64_t (*seek_data)(struct archive_read *, int64_t, int),
    int (*cleanup)(struct archive_read *),
    int (*format_capabilities)(struct archive_read *),
    int (*has_encrypted_entries)(struct archive_read *))
{
	int i, number_slots;

	archive_check_magic(&a->archive,
	    ARCHIVE_READ_MAGIC, ARCHIVE_STATE_NEW,
	    "__archive_read_register_format");

	number_slots = sizeof(a->formats) / sizeof(a->formats[0]);

	for (i = 0; i < number_slots; i++) {
		if (a->formats[i].bid == bid)
			return (ARCHIVE_WARN); /* We've already installed */
		if (a->formats[i].bid == NULL) {
			a->formats[i].bid = bid;
			a->formats[i].options = options;
			a->formats[i].read_header = read_header;
			a->formats[i].read_data = read_data;
			a->formats[i].read_data_skip = read_data_skip;
			a->formats[i].seek_data = seek_data;
			a->formats[i].cleanup = cleanup;
			a->formats[i].data = format_data;
			a->formats[i].name = name;
			a->formats[i].format_capabilties = format_capabilities;
			a->formats[i].has_encrypted_entries = has_encrypted_entries;
			return (ARCHIVE_OK);
		}
	}

	archive_set_error(&a->archive, ENOMEM,
	    "Not enough slots for format registration");
	return (ARCHIVE_FATAL);
}
