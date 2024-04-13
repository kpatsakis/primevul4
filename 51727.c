cosine_seek_read(wtap *wth, gint64 seek_off, struct wtap_pkthdr *phdr,
	Buffer *buf, int *err, gchar **err_info)
{
	char	line[COSINE_LINE_LENGTH];

	if (file_seek(wth->random_fh, seek_off, SEEK_SET, err) == -1)
		return FALSE;

	if (file_gets(line, COSINE_LINE_LENGTH, wth->random_fh) == NULL) {
		*err = file_error(wth->random_fh, err_info);
		if (*err == 0) {
			*err = WTAP_ERR_SHORT_READ;
		}
		return FALSE;
	}

	/* Parse the header and convert the ASCII hex dump to binary data */
	return parse_cosine_packet(wth->random_fh, phdr, buf, line, err,
	    err_info);
}
