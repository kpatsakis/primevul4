int mm_answer_auth2_read_banner(int sock, Buffer *m)
{
	char *banner;

	buffer_clear(m);
	banner = auth2_read_banner();
	buffer_put_cstring(m, banner != NULL ? banner : "");
	mm_request_send(sock, MONITOR_ANS_AUTH2_READ_BANNER, m);
	free(banner);

	return (0);
}
