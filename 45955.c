void sk_clear_memalloc(struct sock *sk)
{
	sock_reset_flag(sk, SOCK_MEMALLOC);
	sk->sk_allocation &= ~__GFP_MEMALLOC;
	static_key_slow_dec(&memalloc_socks);

	/*
	 * SOCK_MEMALLOC is allowed to ignore rmem limits to ensure forward
	 * progress of swapping. However, if SOCK_MEMALLOC is cleared while
	 * it has rmem allocations there is a risk that the user of the
	 * socket cannot make forward progress due to exceeding the rmem
	 * limits. By rights, sk_clear_memalloc() should only be called
	 * on sockets being torn down but warn and reset the accounting if
	 * that assumption breaks.
	 */
	if (WARN_ON(sk->sk_forward_alloc))
		sk_mem_reclaim(sk);
}
