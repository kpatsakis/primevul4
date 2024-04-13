void sctp_transport_walk_stop(struct rhashtable_iter *iter)
{
	rhashtable_walk_stop(iter);
	rhashtable_walk_exit(iter);
}
