archive_read_set_callback_data(struct archive *_a, void *client_data)
{
	return archive_read_set_callback_data2(_a, client_data, 0);
}
