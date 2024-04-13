static bool netlink_tx_is_mmaped(struct sock *sk)
{
	return nlk_sk(sk)->tx_ring.pg_vec != NULL;
}
