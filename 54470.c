static void gluster_free_host(gluster_hostdef *host)
{
	if(!host)
		return;

	switch (host->type) {
	case GLUSTER_TRANSPORT_UNIX:
		free(host->u.uds.socket);
		break;
	case GLUSTER_TRANSPORT_TCP:
	case GLUSTER_TRANSPORT_RDMA:
		free(host->u.inet.addr);
		free(host->u.inet.port);
		break;
	case GLUSTER_TRANSPORT__MAX:
		break;
	}
}
