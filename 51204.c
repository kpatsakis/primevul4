archive_read_prepend_callback_data(struct archive *_a, void *client_data)
{
	return archive_read_add_callback_data(_a, client_data, 0);
}
