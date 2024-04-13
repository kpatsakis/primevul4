static gboolean purple_transfer_request_cb(gpointer data, gint fd, b_input_condition cond)
{
	file_transfer_t *ft = data;
	struct prpl_xfer_data *px = ft->data;

	px->timeout = 0;

	if (ft->write == NULL) {
		ft->write = prpl_xfer_write;
		imcb_file_recv_start(px->ic, ft);
	}

	ft->write_request(ft);

	return FALSE;
}
