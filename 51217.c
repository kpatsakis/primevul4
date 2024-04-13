client_open_proxy(struct archive_read_filter *self)
{
  int r = ARCHIVE_OK;
	if (self->archive->client.opener != NULL)
		r = (self->archive->client.opener)(
		    (struct archive *)self->archive, self->data);
	return (r);
}
