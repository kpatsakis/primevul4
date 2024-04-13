static void atalk_destroy_timer(unsigned long data)
{
	struct sock *sk = (struct sock *)data;

	if (sk_has_allocations(sk)) {
		sk->sk_timer.expires = jiffies + SOCK_DESTROY_TIME;
		add_timer(&sk->sk_timer);
	} else
		sock_put(sk);
}
