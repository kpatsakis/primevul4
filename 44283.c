static int cma_format_hdr(void *hdr, struct rdma_id_private *id_priv)
{
	struct cma_hdr *cma_hdr;

	cma_hdr = hdr;
	cma_hdr->cma_version = CMA_VERSION;
	if (cma_family(id_priv) == AF_INET) {
		struct sockaddr_in *src4, *dst4;

		src4 = (struct sockaddr_in *) cma_src_addr(id_priv);
		dst4 = (struct sockaddr_in *) cma_dst_addr(id_priv);

		cma_set_ip_ver(cma_hdr, 4);
		cma_hdr->src_addr.ip4.addr = src4->sin_addr.s_addr;
		cma_hdr->dst_addr.ip4.addr = dst4->sin_addr.s_addr;
		cma_hdr->port = src4->sin_port;
	} else if (cma_family(id_priv) == AF_INET6) {
		struct sockaddr_in6 *src6, *dst6;

		src6 = (struct sockaddr_in6 *) cma_src_addr(id_priv);
		dst6 = (struct sockaddr_in6 *) cma_dst_addr(id_priv);

		cma_set_ip_ver(cma_hdr, 6);
		cma_hdr->src_addr.ip6 = src6->sin6_addr;
		cma_hdr->dst_addr.ip6 = dst6->sin6_addr;
		cma_hdr->port = src6->sin6_port;
	}
	return 0;
}
