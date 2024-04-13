get_handle(struct sshbuf *queue, int *hp)
{
	u_char *handle;
	int r;
	size_t hlen;

	*hp = -1;
	if ((r = sshbuf_get_string(queue, &handle, &hlen)) != 0)
		return r;
	if (hlen < 256)
		*hp = handle_from_string(handle, hlen);
	free(handle);
	return 0;
}
