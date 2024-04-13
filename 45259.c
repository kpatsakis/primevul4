static void dgram_unhash(struct sock *sk)
{
	write_lock_bh(&dgram_lock);
	if (sk_del_node_init(sk))
		sock_prot_inuse_add(sock_net(sk), sk->sk_prot, -1);
	write_unlock_bh(&dgram_lock);
}
