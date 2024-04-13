static void prpl_xfer_accept(struct file_transfer *ft)
{
	struct prpl_xfer_data *px = ft->data;

	purple_xfer_request_accepted(px->xfer, NULL);
	prpl_xfer_write_request(ft);
}
