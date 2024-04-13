struct tcp_md5sig_key *tcp_md5_do_lookup(const struct sock *sk,
					 const union tcp_md5_addr *addr,
					 int family)
{
	const struct tcp_sock *tp = tcp_sk(sk);
	struct tcp_md5sig_key *key;
	unsigned int size = sizeof(struct in_addr);
	const struct tcp_md5sig_info *md5sig;

	/* caller either holds rcu_read_lock() or socket lock */
	md5sig = rcu_dereference_check(tp->md5sig_info,
				       lockdep_sock_is_held(sk));
	if (!md5sig)
		return NULL;
#if IS_ENABLED(CONFIG_IPV6)
	if (family == AF_INET6)
		size = sizeof(struct in6_addr);
#endif
	hlist_for_each_entry_rcu(key, &md5sig->head, node) {
		if (key->family != family)
			continue;
		if (!memcmp(&key->addr, addr, size))
			return key;
	}
	return NULL;
}
