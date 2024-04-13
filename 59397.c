static void prpl_xfer_canceled(struct file_transfer *ft, char *reason)
{
	struct prpl_xfer_data *px = ft->data;

	if (px->xfer) {
		if (!purple_xfer_is_completed(px->xfer) && !purple_xfer_is_canceled(px->xfer)) {
			purple_xfer_cancel_local(px->xfer);
		}
		px->xfer->ui_data = NULL;
		purple_xfer_unref(px->xfer);
		px->xfer = NULL;
	}
}
