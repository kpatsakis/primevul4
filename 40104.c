static void vcc_def_wakeup(struct sock *sk)
{
	struct socket_wq *wq;

	rcu_read_lock();
	wq = rcu_dereference(sk->sk_wq);
	if (wq_has_sleeper(wq))
		wake_up(&wq->wait);
	rcu_read_unlock();
}
