static int cma_set_qkey(struct rdma_id_private *id_priv, u32 qkey)
{
	struct ib_sa_mcmember_rec rec;
	int ret = 0;

	if (id_priv->qkey) {
		if (qkey && id_priv->qkey != qkey)
			return -EINVAL;
		return 0;
	}

	if (qkey) {
		id_priv->qkey = qkey;
		return 0;
	}

	switch (id_priv->id.ps) {
	case RDMA_PS_UDP:
	case RDMA_PS_IB:
		id_priv->qkey = RDMA_UDP_QKEY;
		break;
	case RDMA_PS_IPOIB:
		ib_addr_get_mgid(&id_priv->id.route.addr.dev_addr, &rec.mgid);
		ret = ib_sa_get_mcmember_rec(id_priv->id.device,
					     id_priv->id.port_num, &rec.mgid,
					     &rec);
		if (!ret)
			id_priv->qkey = be32_to_cpu(rec.qkey);
		break;
	default:
		break;
	}
	return ret;
}
