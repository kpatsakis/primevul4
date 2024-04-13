archive_read_set_skip_callback(struct archive *_a,
    archive_skip_callback *client_skipper)
{
	struct archive_read *a = (struct archive_read *)_a;
	archive_check_magic(_a, ARCHIVE_READ_MAGIC, ARCHIVE_STATE_NEW,
	    "archive_read_set_skip_callback");
	a->client.skipper = client_skipper;
	return ARCHIVE_OK;
}
