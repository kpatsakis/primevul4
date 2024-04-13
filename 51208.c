archive_read_set_open_callback(struct archive *_a,
    archive_open_callback *client_opener)
{
	struct archive_read *a = (struct archive_read *)_a;
	archive_check_magic(_a, ARCHIVE_READ_MAGIC, ARCHIVE_STATE_NEW,
	    "archive_read_set_open_callback");
	a->client.opener = client_opener;
	return ARCHIVE_OK;
}
