static void prplcb_xfer_cancel_remote(PurpleXfer *xfer)
{
	struct prpl_xfer_data *px = xfer->ui_data;

	if (px && px->ft) {
		imcb_file_canceled(px->ic, px->ft, "Canceled by remote end");
	} else if (px) {
		/* px->ft == NULL for sends, because of the two stages. :-/ */
		imcb_error(px->ic, "File transfer cancelled by remote end");
	}
}
