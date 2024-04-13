static void pfkey_remove(struct sock *sk)
{
	mutex_lock(&pfkey_mutex);
	sk_del_node_init_rcu(sk);
	mutex_unlock(&pfkey_mutex);
}
