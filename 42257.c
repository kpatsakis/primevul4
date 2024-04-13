int skb_kill_datagram(struct sock *sk, struct sk_buff *skb, unsigned int flags)
{
	int err = 0;

	if (flags & MSG_PEEK) {
		err = -ENOENT;
		spin_lock_bh(&sk->sk_receive_queue.lock);
		if (skb == skb_peek(&sk->sk_receive_queue)) {
			__skb_unlink(skb, &sk->sk_receive_queue);
			atomic_dec(&skb->users);
			err = 0;
		}
		spin_unlock_bh(&sk->sk_receive_queue.lock);
	}

	kfree_skb(skb);
	atomic_inc(&sk->sk_drops);
	sk_mem_reclaim_partial(sk);

	return err;
}
