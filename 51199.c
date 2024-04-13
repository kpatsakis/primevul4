archive_read_format_capabilities(struct archive *_a)
{
	struct archive_read *a = (struct archive_read *)_a;
	if (a && a->format && a->format->format_capabilties) {
		return (a->format->format_capabilties)(a);
	}
	return ARCHIVE_READ_FORMAT_CAPS_NONE;
}
