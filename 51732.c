wtap_open_return_val toshiba_open(wtap *wth, int *err, gchar **err_info)
{
	/* Look for Toshiba header */
	if (!toshiba_check_file_type(wth, err, err_info)) {
		if (*err != 0 && *err != WTAP_ERR_SHORT_READ)
			return WTAP_OPEN_ERROR;
		return WTAP_OPEN_NOT_MINE;
	}

	wth->file_encap = WTAP_ENCAP_PER_PACKET;
	wth->file_type_subtype = WTAP_FILE_TYPE_SUBTYPE_TOSHIBA;
	wth->snapshot_length = 0; /* not known */
	wth->subtype_read = toshiba_read;
	wth->subtype_seek_read = toshiba_seek_read;
	wth->file_tsprec = WTAP_TSPREC_CSEC;

	return WTAP_OPEN_MINE;
}
