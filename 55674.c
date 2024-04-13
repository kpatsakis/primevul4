add_link(struct archive_read *a, struct xar *xar, struct xar_file *file)
{
	struct hdlink *hdlink;

	for (hdlink = xar->hdlink_list; hdlink != NULL; hdlink = hdlink->next) {
		if (hdlink->id == file->link) {
			file->hdnext = hdlink->files;
			hdlink->cnt++;
			hdlink->files = file;
			return (ARCHIVE_OK);
		}
	}
	hdlink = malloc(sizeof(*hdlink));
	if (hdlink == NULL) {
		archive_set_error(&a->archive, ENOMEM, "Out of memory");
		return (ARCHIVE_FATAL);
	}
	file->hdnext = NULL;
	hdlink->id = file->link;
	hdlink->cnt = 1;
	hdlink->files = file;
	hdlink->next = xar->hdlink_list;
	xar->hdlink_list = hdlink;
	return (ARCHIVE_OK);
}
