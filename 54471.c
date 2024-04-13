static void gluster_free_server(gluster_server **hosts)
{
	if (!*hosts)
		return;
	free((*hosts)->volname);
	free((*hosts)->path);

	gluster_free_host((*hosts)->server);
	free((*hosts)->server);
	(*hosts)->server = NULL;
	free(*hosts);
	*hosts = NULL;
}
