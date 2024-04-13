archive_read_set_close_callback(struct archive *_a,
    archive_close_callback *client_closer)
{
	struct archive_read *a = (struct archive_read *)_a;
	archive_check_magic(_a, ARCHIVE_READ_MAGIC, ARCHIVE_STATE_NEW,
	    "archive_read_set_close_callback");
	a->client.closer = client_closer;
	return ARCHIVE_OK;
}
