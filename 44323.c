static int cma_save_net_info(struct rdma_cm_id *id, struct rdma_cm_id *listen_id,
			     struct ib_cm_event *ib_event)
{
	struct cma_hdr *hdr;

	if ((listen_id->route.addr.src_addr.ss_family == AF_IB) &&
	    (ib_event->event == IB_CM_REQ_RECEIVED)) {
		cma_save_ib_info(id, listen_id, ib_event->param.req_rcvd.primary_path);
		return 0;
	}

	hdr = ib_event->private_data;
	if (hdr->cma_version != CMA_VERSION)
		return -EINVAL;

	switch (cma_get_ip_ver(hdr)) {
	case 4:
		cma_save_ip4_info(id, listen_id, hdr);
		break;
	case 6:
		cma_save_ip6_info(id, listen_id, hdr);
		break;
	default:
		return -EINVAL;
	}
	return 0;
}
