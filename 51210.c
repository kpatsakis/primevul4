archive_read_set_seek_callback(struct archive *_a,
    archive_seek_callback *client_seeker)
{
	struct archive_read *a = (struct archive_read *)_a;
	archive_check_magic(_a, ARCHIVE_READ_MAGIC, ARCHIVE_STATE_NEW,
	    "archive_read_set_seek_callback");
	a->client.seeker = client_seeker;
	return ARCHIVE_OK;
}
