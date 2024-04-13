static void cma_bind_port(struct rdma_bind_list *bind_list,
			  struct rdma_id_private *id_priv)
{
	struct sockaddr *addr;
	struct sockaddr_ib *sib;
	u64 sid, mask;
	__be16 port;

	addr = cma_src_addr(id_priv);
	port = htons(bind_list->port);

	switch (addr->sa_family) {
	case AF_INET:
		((struct sockaddr_in *) addr)->sin_port = port;
		break;
	case AF_INET6:
		((struct sockaddr_in6 *) addr)->sin6_port = port;
		break;
	case AF_IB:
		sib = (struct sockaddr_ib *) addr;
		sid = be64_to_cpu(sib->sib_sid);
		mask = be64_to_cpu(sib->sib_sid_mask);
		sib->sib_sid = cpu_to_be64((sid & mask) | (u64) ntohs(port));
		sib->sib_sid_mask = cpu_to_be64(~0ULL);
		break;
	}
	id_priv->bind_list = bind_list;
	hlist_add_head(&id_priv->node, &bind_list->owners);
}
