client_read_proxy(struct archive_read_filter *self, const void **buff)
{
	ssize_t r;
	r = (self->archive->client.reader)(&self->archive->archive,
	    self->data, buff);
	return (r);
}
