int sctp_transport_walk_start(struct rhashtable_iter *iter)
{
	int err;

	rhltable_walk_enter(&sctp_transport_hashtable, iter);

	err = rhashtable_walk_start(iter);
	if (err && err != -EAGAIN) {
		rhashtable_walk_stop(iter);
		rhashtable_walk_exit(iter);
		return err;
	}

	return 0;
}
