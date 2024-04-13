static bool netlink_rx_is_mmaped(struct sock *sk)
{
	return nlk_sk(sk)->rx_ring.pg_vec != NULL;
}
