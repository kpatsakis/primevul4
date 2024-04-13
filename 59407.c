void purple_transfer_request(struct im_connection *ic, file_transfer_t *ft, char *handle)
{
	struct prpl_xfer_data *px = g_new0(struct prpl_xfer_data, 1);
	struct purple_data *pd;
	char *dir, *basename;

	ft->data = px;
	px->ft = ft;
	px->ft->free = prpl_xfer_free;

	dir = g_strdup("/tmp/bitlbee-purple-ft.XXXXXX");
	if (!mkdtemp(dir)) {
		imcb_error(ic, "Could not create temporary file for file transfer");
		g_free(px);
		g_free(dir);
		return;
	}

	if ((basename = strrchr(ft->file_name, '/'))) {
		basename++;
	} else {
		basename = ft->file_name;
	}
	px->fn = g_strdup_printf("%s/%s", dir, basename);
	px->fd = open(px->fn, O_WRONLY | O_CREAT, 0600);
	g_free(dir);

	if (px->fd < 0) {
		imcb_error(ic, "Could not create temporary file for file transfer");
		g_free(px);
		g_free(px->fn);
		return;
	}

	px->ic = ic;
	px->handle = g_strdup(handle);

	pd = px->ic->proto_data;
	pd->filetransfers = g_slist_prepend(pd->filetransfers, px);

	imcb_log(ic,
	         "Due to libpurple limitations, the file has to be cached locally before proceeding with the actual file transfer. Please wait...");

	px->timeout = b_timeout_add(0, purple_transfer_request_cb, ft);
}
