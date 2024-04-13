xattr_free(struct xattr *xattr)
{
	archive_string_free(&(xattr->name));
	free(xattr);
}
