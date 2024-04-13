static gboolean prpl_xfer_write_request(struct file_transfer *ft)
{
	struct prpl_xfer_data *px = ft->data;

	px->ui_wants_data = TRUE;
	try_write_to_ui(ft, 0, 0);

	return FALSE;
}
