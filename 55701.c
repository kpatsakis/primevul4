xattr_new(struct archive_read *a, struct xar *xar, struct xmlattr_list *list)
{
	struct xattr *xattr, **nx;
	struct xmlattr *attr;

	xattr = calloc(1, sizeof(*xattr));
	if (xattr == NULL) {
		archive_set_error(&a->archive, ENOMEM, "Out of memory");
		return (ARCHIVE_FATAL);
	}
	xar->xattr = xattr;
	for (attr = list->first; attr != NULL; attr = attr->next) {
		if (strcmp(attr->name, "id") == 0)
			xattr->id = atol10(attr->value, strlen(attr->value));
	}
	/* Chain to xattr list. */
	for (nx = &(xar->file->xattr_list);
	    *nx != NULL; nx = &((*nx)->next)) {
		if (xattr->id < (*nx)->id)
			break;
	}
	xattr->next = *nx;
	*nx = xattr;

	return (ARCHIVE_OK);
}
