gboolean tcmulib_callback(GIOChannel *source,
			  GIOCondition condition,
			  gpointer data)
{
	struct tcmulib_context *ctx = data;

	tcmulib_master_fd_ready(ctx);

	return TRUE;
}
