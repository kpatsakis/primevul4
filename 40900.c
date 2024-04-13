struct tcp_md5sig_pool *__tcp_get_md5sig_pool(int cpu)
{
	struct tcp_md5sig_pool * __percpu *p;
	spin_lock_bh(&tcp_md5sig_pool_lock);
	p = tcp_md5sig_pool;
	if (p)
		tcp_md5sig_users++;
	spin_unlock_bh(&tcp_md5sig_pool_lock);
	return (p ? *per_cpu_ptr(p, cpu) : NULL);
}
