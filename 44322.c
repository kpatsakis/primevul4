static void cma_save_ip6_info(struct rdma_cm_id *id, struct rdma_cm_id *listen_id,
			      struct cma_hdr *hdr)
{
	struct sockaddr_in6 *listen6, *ip6;

	listen6 = (struct sockaddr_in6 *) &listen_id->route.addr.src_addr;
	ip6 = (struct sockaddr_in6 *) &id->route.addr.src_addr;
	ip6->sin6_family = listen6->sin6_family;
	ip6->sin6_addr = hdr->dst_addr.ip6;
	ip6->sin6_port = listen6->sin6_port;

	ip6 = (struct sockaddr_in6 *) &id->route.addr.dst_addr;
	ip6->sin6_family = listen6->sin6_family;
	ip6->sin6_addr = hdr->src_addr.ip6;
	ip6->sin6_port = hdr->port;
}
