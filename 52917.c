static void option_cat_blob_fd(const char *fd)
{
	unsigned long n = ulong_arg("--cat-blob-fd", fd);
	if (n > (unsigned long) INT_MAX)
		die("--cat-blob-fd cannot exceed %d", INT_MAX);
	cat_blob_fd = (int) n;
}
