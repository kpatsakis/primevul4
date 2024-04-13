void release_sock(struct sock *sk)
{
	/*
	 * The sk_lock has mutex_unlock() semantics:
	 */
	mutex_release(&sk->sk_lock.dep_map, 1, _RET_IP_);

	spin_lock_bh(&sk->sk_lock.slock);
	if (sk->sk_backlog.tail)
		__release_sock(sk);

	if (sk->sk_prot->release_cb)
		sk->sk_prot->release_cb(sk);

	sk->sk_lock.owned = 0;
	if (waitqueue_active(&sk->sk_lock.wq))
		wake_up(&sk->sk_lock.wq);
	spin_unlock_bh(&sk->sk_lock.slock);
}
