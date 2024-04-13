client_switch_proxy(struct archive_read_filter *self, unsigned int iindex)
{
  int r1 = ARCHIVE_OK, r2 = ARCHIVE_OK;
	void *data2 = NULL;

	/* Don't do anything if already in the specified data node */
	if (self->archive->client.cursor == iindex)
		return (ARCHIVE_OK);

	self->archive->client.cursor = iindex;
	data2 = self->archive->client.dataset[self->archive->client.cursor].data;
	if (self->archive->client.switcher != NULL)
	{
		r1 = r2 = (self->archive->client.switcher)
			((struct archive *)self->archive, self->data, data2);
		self->data = data2;
	}
	else
	{
		/* Attempt to call close and open instead */
		if (self->archive->client.closer != NULL)
			r1 = (self->archive->client.closer)
				((struct archive *)self->archive, self->data);
		self->data = data2;
		if (self->archive->client.opener != NULL)
			r2 = (self->archive->client.opener)
				((struct archive *)self->archive, self->data);
	}
	return (r1 < r2) ? r1 : r2;
}
