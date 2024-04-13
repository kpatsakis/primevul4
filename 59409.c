gboolean try_write_to_ui(gpointer data, gint fd, b_input_condition cond)
{
	struct file_transfer *ft = data;
	struct prpl_xfer_data *px = ft->data;
	struct stat fs;
	off_t tx_bytes;

	/* If we don't have the file opened yet, there's no data so wait. */
	if (px->fd < 0 || !px->ui_wants_data) {
		return FALSE;
	}

	tx_bytes = lseek(px->fd, 0, SEEK_CUR);
	fstat(px->fd, &fs);

	if (fs.st_size > tx_bytes) {
		char buf[1024];
		size_t n = MIN(fs.st_size - tx_bytes, sizeof(buf));

		if (read(px->fd, buf, n) == n && ft->write(ft, buf, n)) {
			px->ui_wants_data = FALSE;
		} else {
			purple_xfer_cancel_local(px->xfer);
			imcb_file_canceled(px->ic, ft, "Read error");
		}
	}

	if (lseek(px->fd, 0, SEEK_CUR) == px->xfer->size) {
		/*purple_xfer_end( px->xfer );*/
		imcb_file_finished(px->ic, ft);
	}

	return FALSE;
}
