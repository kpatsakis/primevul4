static void prplcb_xfer_new(PurpleXfer *xfer)
{
	purple_xfer_ref(xfer);

	if (purple_xfer_get_type(xfer) == PURPLE_XFER_RECEIVE) {
		struct prpl_xfer_data *px = g_new0(struct prpl_xfer_data, 1);
		struct purple_data *pd;

		xfer->ui_data = px;
		px->xfer = xfer;
		px->fn = mktemp(g_strdup("/tmp/bitlbee-purple-ft.XXXXXX"));
		px->fd = -1;
		px->ic = purple_ic_by_pa(xfer->account);

		pd = px->ic->proto_data;
		pd->filetransfers = g_slist_prepend(pd->filetransfers, px);

		purple_xfer_set_local_filename(xfer, px->fn);

		/* Sadly the xfer struct is still empty ATM so come back after
		   the caller is done. */
		b_timeout_add(0, prplcb_xfer_new_send_cb, xfer);
	} else {
		struct file_transfer *ft = next_ft;
		struct prpl_xfer_data *px = ft->data;

		xfer->ui_data = px;
		px->xfer = xfer;

		next_ft = NULL;
	}
}
