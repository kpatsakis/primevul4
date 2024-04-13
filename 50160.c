void release_sock(struct sock *sk)
{
	spin_lock_bh(&sk->sk_lock.slock);
	if (sk->sk_backlog.tail)
		__release_sock(sk);

	/* Warning : release_cb() might need to release sk ownership,
	 * ie call sock_release_ownership(sk) before us.
	 */
	if (sk->sk_prot->release_cb)
		sk->sk_prot->release_cb(sk);

	sock_release_ownership(sk);
	if (waitqueue_active(&sk->sk_lock.wq))
		wake_up(&sk->sk_lock.wq);
	spin_unlock_bh(&sk->sk_lock.slock);
}
