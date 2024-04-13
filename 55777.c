static unsigned int tcp_xmit_size_goal(struct sock *sk, u32 mss_now,
				       int large_allowed)
{
	struct tcp_sock *tp = tcp_sk(sk);
	u32 new_size_goal, size_goal;

	if (!large_allowed || !sk_can_gso(sk))
		return mss_now;

	/* Note : tcp_tso_autosize() will eventually split this later */
	new_size_goal = sk->sk_gso_max_size - 1 - MAX_TCP_HEADER;
	new_size_goal = tcp_bound_to_half_wnd(tp, new_size_goal);

	/* We try hard to avoid divides here */
	size_goal = tp->gso_segs * mss_now;
	if (unlikely(new_size_goal < size_goal ||
		     new_size_goal >= size_goal + mss_now)) {
		tp->gso_segs = min_t(u16, new_size_goal / mss_now,
				     sk->sk_gso_max_segs);
		size_goal = tp->gso_segs * mss_now;
	}

	return max(size_goal, mss_now);
}
