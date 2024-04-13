file_new(struct archive_read *a, struct xar *xar, struct xmlattr_list *list)
{
	struct xar_file *file;
	struct xmlattr *attr;

	file = calloc(1, sizeof(*file));
	if (file == NULL) {
		archive_set_error(&a->archive, ENOMEM, "Out of memory");
		return (ARCHIVE_FATAL);
	}
	file->parent = xar->file;
	file->mode = 0777 | AE_IFREG;
	file->atime = time(NULL);
	file->mtime = time(NULL);
	xar->file = file;
	xar->xattr = NULL;
	for (attr = list->first; attr != NULL; attr = attr->next) {
		if (strcmp(attr->name, "id") == 0)
			file->id = atol10(attr->value, strlen(attr->value));
	}
	file->nlink = 1;
	if (heap_add_entry(a, &(xar->file_queue), file) != ARCHIVE_OK)
		return (ARCHIVE_FATAL);
	return (ARCHIVE_OK);
}
