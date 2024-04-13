static void prplcb_xfer_destroy(PurpleXfer *xfer)
{
	struct prpl_xfer_data *px = xfer->ui_data;

	if (px) {
		px->xfer = NULL;
	}
}
