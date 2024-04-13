xar_cleanup(struct archive_read *a)
{
	struct xar *xar;
	struct hdlink *hdlink;
	int i;
	int r;

	xar = (struct xar *)(a->format->data);
	checksum_cleanup(a);
	r = decompression_cleanup(a);
	hdlink = xar->hdlink_list;
	while (hdlink != NULL) {
		struct hdlink *next = hdlink->next;

		free(hdlink);
		hdlink = next;
	}
	for (i = 0; i < xar->file_queue.used; i++)
		file_free(xar->file_queue.files[i]);
	free(xar->file_queue.files);
	while (xar->unknowntags != NULL) {
		struct unknown_tag *tag;

		tag = xar->unknowntags;
		xar->unknowntags = tag->next;
		archive_string_free(&(tag->name));
		free(tag);
	}
	free(xar->outbuff);
	free(xar);
	a->format->data = NULL;
	return (r);
}
