client_close_proxy(struct archive_read_filter *self)
{
	int r = ARCHIVE_OK, r2;
	unsigned int i;

	if (self->archive->client.closer == NULL)
		return (r);
	for (i = 0; i < self->archive->client.nodes; i++)
	{
		r2 = (self->archive->client.closer)
			((struct archive *)self->archive,
				self->archive->client.dataset[i].data);
		if (r > r2)
			r = r2;
	}
	return (r);
}
