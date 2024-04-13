archive_read_set_read_callback(struct archive *_a,
    archive_read_callback *client_reader)
{
	struct archive_read *a = (struct archive_read *)_a;
	archive_check_magic(_a, ARCHIVE_READ_MAGIC, ARCHIVE_STATE_NEW,
	    "archive_read_set_read_callback");
	a->client.reader = client_reader;
	return ARCHIVE_OK;
}
