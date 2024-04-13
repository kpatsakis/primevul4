static bool __fanout_id_is_free(struct sock *sk, u16 candidate_id)
{
	struct packet_fanout *f;

	list_for_each_entry(f, &fanout_list, list) {
		if (f->id == candidate_id &&
		    read_pnet(&f->net) == sock_net(sk)) {
			return false;
		}
	}
	return true;
}
