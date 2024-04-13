toshiba_seek_read(wtap *wth, gint64 seek_off,
	struct wtap_pkthdr *phdr, Buffer *buf,
	int *err, gchar **err_info)
{
	if (file_seek(wth->random_fh, seek_off - 1, SEEK_SET, err) == -1)
		return FALSE;

	if (!parse_toshiba_packet(wth->random_fh, phdr, buf, err, err_info)) {
		if (*err == 0)
			*err = WTAP_ERR_SHORT_READ;
		return FALSE;
	}
	return TRUE;
}
