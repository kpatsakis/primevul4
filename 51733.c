static gboolean toshiba_read(wtap *wth, int *err, gchar **err_info,
    gint64 *data_offset)
{
	gint64	offset;

	/* Find the next packet */
	offset = toshiba_seek_next_packet(wth, err, err_info);
	if (offset < 1)
		return FALSE;
	*data_offset = offset;

	/* Parse the packet */
	return parse_toshiba_packet(wth->fh, &wth->phdr, wth->frame_buffer,
	    err, err_info);
}
