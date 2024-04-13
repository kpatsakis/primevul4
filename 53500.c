file_pop_buffer(struct magic_set *ms, file_pushbuf_t *pb)
{
	char *rbuf;

	if (ms->event_flags & EVENT_HAD_ERR) {
		free(pb->buf);
		free(pb);
		return NULL;
	}

	rbuf = ms->o.buf;

	ms->o.buf = pb->buf;
	ms->offset = pb->offset;

	free(pb);
	return rbuf;
}
