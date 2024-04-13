static int cma_translate_addr(struct sockaddr *addr, struct rdma_dev_addr *dev_addr)
{
	int ret;

	if (addr->sa_family != AF_IB) {
		ret = rdma_translate_ip(addr, dev_addr, NULL);
	} else {
		cma_translate_ib((struct sockaddr_ib *) addr, dev_addr);
		ret = 0;
	}

	return ret;
}
