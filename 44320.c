static void cma_save_ib_info(struct rdma_cm_id *id, struct rdma_cm_id *listen_id,
			     struct ib_sa_path_rec *path)
{
	struct sockaddr_ib *listen_ib, *ib;

	listen_ib = (struct sockaddr_ib *) &listen_id->route.addr.src_addr;
	ib = (struct sockaddr_ib *) &id->route.addr.src_addr;
	ib->sib_family = listen_ib->sib_family;
	ib->sib_pkey = path->pkey;
	ib->sib_flowinfo = path->flow_label;
	memcpy(&ib->sib_addr, &path->sgid, 16);
	ib->sib_sid = listen_ib->sib_sid;
	ib->sib_sid_mask = cpu_to_be64(0xffffffffffffffffULL);
	ib->sib_scope_id = listen_ib->sib_scope_id;

	ib = (struct sockaddr_ib *) &id->route.addr.dst_addr;
	ib->sib_family = listen_ib->sib_family;
	ib->sib_pkey = path->pkey;
	ib->sib_flowinfo = path->flow_label;
	memcpy(&ib->sib_addr, &path->dgid, 16);
}
