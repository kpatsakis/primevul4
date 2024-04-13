static void cma_translate_ib(struct sockaddr_ib *sib, struct rdma_dev_addr *dev_addr)
{
	dev_addr->dev_type = ARPHRD_INFINIBAND;
	rdma_addr_set_sgid(dev_addr, (union ib_gid *) &sib->sib_addr);
	ib_addr_set_pkey(dev_addr, ntohs(sib->sib_pkey));
}
