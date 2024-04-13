static void cat_blob_write(const char *buf, unsigned long size)
{
	if (write_in_full(cat_blob_fd, buf, size) != size)
		die_errno("Write to frontend failed");
}
