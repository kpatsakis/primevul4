int tcp_md5_do_del(struct sock *sk, const union tcp_md5_addr *addr, int family)
{
	struct tcp_md5sig_key *key;

	key = tcp_md5_do_lookup(sk, addr, family);
	if (!key)
		return -ENOENT;
	hlist_del_rcu(&key->node);
	atomic_sub(sizeof(*key), &sk->sk_omem_alloc);
	kfree_rcu(key, rcu);
	return 0;
}
