archive_read_append_callback_data(struct archive *_a, void *client_data)
{
	struct archive_read *a = (struct archive_read *)_a;
	return archive_read_add_callback_data(_a, client_data, a->client.nodes);
}
