void purple_transfer_cancel_all(struct im_connection *ic)
{
	struct purple_data *pd = ic->proto_data;

	while (pd->filetransfers) {
		struct prpl_xfer_data *px = pd->filetransfers->data;

		if (px->ft) {
			imcb_file_canceled(ic, px->ft, "Logging out");
		}

		pd->filetransfers = g_slist_remove(pd->filetransfers, px);
	}
}
