static void purple_transfer_forward(struct file_transfer *ft)
{
	struct prpl_xfer_data *px = ft->data;
	struct purple_data *pd = px->ic->proto_data;

	/* xfer_new() will pick up this variable. It's a hack but we're not
	   multi-threaded anyway. */
	next_ft = ft;
	serv_send_file(purple_account_get_connection(pd->account),
                   px->handle, px->fn);
}
