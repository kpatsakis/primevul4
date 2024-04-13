static void prpl_xfer_free(struct file_transfer *ft)
{
	struct prpl_xfer_data *px = ft->data;
	struct purple_data *pd = px->ic->proto_data;

	pd->filetransfers = g_slist_remove(pd->filetransfers, px);

	if (px->xfer) {
		px->xfer->ui_data = NULL;
		purple_xfer_unref(px->xfer);
	}

	if (px->timeout) {
		b_event_remove(px->timeout);
	}

	g_free(px->fn);
	g_free(px->handle);
	if (px->fd >= 0) {
		close(px->fd);
	}
	g_free(px);
}
