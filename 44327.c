static void cma_set_compare_data(enum rdma_port_space ps, struct sockaddr *addr,
				 struct ib_cm_compare_data *compare)
{
	struct cma_hdr *cma_data, *cma_mask;
	__be32 ip4_addr;
	struct in6_addr ip6_addr;

	memset(compare, 0, sizeof *compare);
	cma_data = (void *) compare->data;
	cma_mask = (void *) compare->mask;

	switch (addr->sa_family) {
	case AF_INET:
		ip4_addr = ((struct sockaddr_in *) addr)->sin_addr.s_addr;
		cma_set_ip_ver(cma_data, 4);
		cma_set_ip_ver(cma_mask, 0xF);
		if (!cma_any_addr(addr)) {
			cma_data->dst_addr.ip4.addr = ip4_addr;
			cma_mask->dst_addr.ip4.addr = htonl(~0);
		}
		break;
	case AF_INET6:
		ip6_addr = ((struct sockaddr_in6 *) addr)->sin6_addr;
		cma_set_ip_ver(cma_data, 6);
		cma_set_ip_ver(cma_mask, 0xF);
		if (!cma_any_addr(addr)) {
			cma_data->dst_addr.ip6 = ip6_addr;
			memset(&cma_mask->dst_addr.ip6, 0xFF,
			       sizeof cma_mask->dst_addr.ip6);
		}
		break;
	default:
		break;
	}
}
