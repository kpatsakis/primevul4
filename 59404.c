static void prplcb_xfer_progress(PurpleXfer *xfer, double percent)
{
	struct prpl_xfer_data *px = xfer->ui_data;

	if (px == NULL) {
		return;
	}

	if (purple_xfer_get_type(xfer) == PURPLE_XFER_SEND) {
		if (*px->fn) {
			char *slash;

			unlink(px->fn);
			if ((slash = strrchr(px->fn, '/'))) {
				*slash = '\0';
				rmdir(px->fn);
			}
			*px->fn = '\0';
		}

		return;
	}

	if (px->fd == -1 && percent > 0) {
		/* Weeeeeeeee, we're getting data! That means the file exists
		   by now so open it and start sending to the UI. */
		px->fd = open(px->fn, O_RDONLY);

		/* Unlink it now, because we don't need it after this. */
		unlink(px->fn);
	}

	if (percent < 1) {
		try_write_to_ui(px->ft, 0, 0);
	} else {
		/* Another nice problem: If we have the whole file, it only
		   gets closed when we return. Problem: There may still be
		   stuff buffered and not written, we'll only see it after
		   the caller close()s the file. So poll the file after that. */
		b_timeout_add(0, try_write_to_ui, px->ft);
	}
}
