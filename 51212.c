archive_read_set_switch_callback(struct archive *_a,
    archive_switch_callback *client_switcher)
{
	struct archive_read *a = (struct archive_read *)_a;
	archive_check_magic(_a, ARCHIVE_READ_MAGIC, ARCHIVE_STATE_NEW,
	    "archive_read_set_switch_callback");
	a->client.switcher = client_switcher;
	return ARCHIVE_OK;
}
