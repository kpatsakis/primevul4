struct sctp_transport *sctp_transport_get_idx(struct net *net,
					      struct rhashtable_iter *iter,
					      int pos)
{
	void *obj = SEQ_START_TOKEN;

	while (pos && (obj = sctp_transport_get_next(net, iter)) &&
	       !IS_ERR(obj))
		pos--;

	return obj;
}
