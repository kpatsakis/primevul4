int sctp_for_each_transport(int (*cb)(struct sctp_transport *, void *),
			    int (*cb_done)(struct sctp_transport *, void *),
			    struct net *net, int *pos, void *p) {
	struct rhashtable_iter hti;
	struct sctp_transport *tsp;
	int ret;

again:
	ret = sctp_transport_walk_start(&hti);
	if (ret)
		return ret;

	tsp = sctp_transport_get_idx(net, &hti, *pos + 1);
	for (; !IS_ERR_OR_NULL(tsp); tsp = sctp_transport_get_next(net, &hti)) {
		if (!sctp_transport_hold(tsp))
			continue;
		ret = cb(tsp, p);
		if (ret)
			break;
		(*pos)++;
		sctp_transport_put(tsp);
	}
	sctp_transport_walk_stop(&hti);

	if (ret) {
		if (cb_done && !cb_done(tsp, p)) {
			(*pos)++;
			sctp_transport_put(tsp);
			goto again;
		}
		sctp_transport_put(tsp);
	}

	return ret;
}
