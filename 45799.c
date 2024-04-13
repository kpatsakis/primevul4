static void x25_kill_by_device(struct net_device *dev)
{
	struct sock *s;

	write_lock_bh(&x25_list_lock);

	sk_for_each(s, &x25_list)
		if (x25_sk(s)->neighbour && x25_sk(s)->neighbour->dev == dev)
			x25_disconnect(s, ENETUNREACH, 0, 0);

	write_unlock_bh(&x25_list_lock);
}
