static gboolean prpl_xfer_write(struct file_transfer *ft, char *buffer, unsigned int len)
{
	struct prpl_xfer_data *px = ft->data;

	if (write(px->fd, buffer, len) != len) {
		imcb_file_canceled(px->ic, ft, "Error while writing temporary file");
		return FALSE;
	}

	if (lseek(px->fd, 0, SEEK_CUR) >= ft->file_size) {
		close(px->fd);
		px->fd = -1;

		purple_transfer_forward(ft);
		imcb_file_finished(px->ic, ft);
		px->ft = NULL;
	} else {
		px->timeout = b_timeout_add(0, purple_transfer_request_cb, ft);
	}

	return TRUE;
}
