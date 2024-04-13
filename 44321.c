static void cma_save_ip4_info(struct rdma_cm_id *id, struct rdma_cm_id *listen_id,
			      struct cma_hdr *hdr)
{
	struct sockaddr_in *listen4, *ip4;

	listen4 = (struct sockaddr_in *) &listen_id->route.addr.src_addr;
	ip4 = (struct sockaddr_in *) &id->route.addr.src_addr;
	ip4->sin_family = listen4->sin_family;
	ip4->sin_addr.s_addr = hdr->dst_addr.ip4.addr;
	ip4->sin_port = listen4->sin_port;

	ip4 = (struct sockaddr_in *) &id->route.addr.dst_addr;
	ip4->sin_family = listen4->sin_family;
	ip4->sin_addr.s_addr = hdr->src_addr.ip4.addr;
	ip4->sin_port = hdr->port;
}
