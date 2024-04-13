__tcp_alloc_md5sig_pool(struct sock *sk)
{
	int cpu;
	struct tcp_md5sig_pool * __percpu *pool;

	pool = alloc_percpu(struct tcp_md5sig_pool *);
	if (!pool)
		return NULL;

	for_each_possible_cpu(cpu) {
		struct tcp_md5sig_pool *p;
		struct crypto_hash *hash;

		p = kzalloc(sizeof(*p), sk->sk_allocation);
		if (!p)
			goto out_free;
		*per_cpu_ptr(pool, cpu) = p;

		hash = crypto_alloc_hash("md5", 0, CRYPTO_ALG_ASYNC);
		if (!hash || IS_ERR(hash))
			goto out_free;

		p->md5_desc.tfm = hash;
	}
	return pool;
out_free:
	__tcp_free_md5sig_pool(pool);
	return NULL;
}
