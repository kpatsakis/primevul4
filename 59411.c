archive_read_format_lha_cleanup(struct archive_read *a)
{
	struct lha *lha = (struct lha *)(a->format->data);

	lzh_decode_free(&(lha->strm));
	archive_string_free(&(lha->dirname));
	archive_string_free(&(lha->filename));
	archive_string_free(&(lha->uname));
	archive_string_free(&(lha->gname));
	archive_wstring_free(&(lha->ws));
	free(lha);
	(a->format->data) = NULL;
	return (ARCHIVE_OK);
}
