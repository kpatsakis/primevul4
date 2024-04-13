static int dgram_ioctl(struct sock *sk, int cmd, unsigned long arg)
{
	switch (cmd) {
	case SIOCOUTQ:
	{
		int amount = sk_wmem_alloc_get(sk);

		return put_user(amount, (int __user *)arg);
	}

	case SIOCINQ:
	{
		struct sk_buff *skb;
		unsigned long amount;

		amount = 0;
		spin_lock_bh(&sk->sk_receive_queue.lock);
		skb = skb_peek(&sk->sk_receive_queue);
		if (skb != NULL) {
			/*
			 * We will only return the amount
			 * of this packet since that is all
			 * that will be read.
			 */
			/* FIXME: parse the header for more correct value */
			amount = skb->len - (3+8+8);
		}
		spin_unlock_bh(&sk->sk_receive_queue.lock);
		return put_user(amount, (int __user *)arg);
	}

	}
	return -ENOIOCTLCMD;
}
