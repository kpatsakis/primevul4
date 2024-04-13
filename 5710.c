void list_peers(int fd)
{
	char *data;
	unsigned int olen;
	struct boothc_hdr_msg hdr;

	if (format_peers(&data, &olen) < 0)
		goto out;

	init_header(&hdr.header, CL_LIST, 0, 0, RLT_SUCCESS, 0, sizeof(hdr) + olen);
	(void)send_header_plus(fd, &hdr, data, olen);

out:
	if (data)
		free(data);
}