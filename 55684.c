file_free(struct xar_file *file)
{
	struct xattr *xattr;

	archive_string_free(&(file->pathname));
	archive_string_free(&(file->symlink));
	archive_string_free(&(file->uname));
	archive_string_free(&(file->gname));
	archive_string_free(&(file->hardlink));
	xattr = file->xattr_list;
	while (xattr != NULL) {
		struct xattr *next;

		next = xattr->next;
		xattr_free(xattr);
		xattr = next;
	}

	free(file);
}
