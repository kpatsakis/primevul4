static int bnep_session(void *arg)
{
	struct bnep_session *s = arg;
	struct net_device *dev = s->dev;
	struct sock *sk = s->sock->sk;
	struct sk_buff *skb;
	wait_queue_t wait;

	BT_DBG("");

	set_user_nice(current, -15);

	init_waitqueue_entry(&wait, current);
	add_wait_queue(sk_sleep(sk), &wait);
	while (1) {
		set_current_state(TASK_INTERRUPTIBLE);

		if (atomic_read(&s->terminate))
			break;
		/* RX */
		while ((skb = skb_dequeue(&sk->sk_receive_queue))) {
			skb_orphan(skb);
			if (!skb_linearize(skb))
				bnep_rx_frame(s, skb);
			else
				kfree_skb(skb);
		}

		if (sk->sk_state != BT_CONNECTED)
			break;

		/* TX */
		while ((skb = skb_dequeue(&sk->sk_write_queue)))
			if (bnep_tx_frame(s, skb))
				break;
		netif_wake_queue(dev);

		schedule();
	}
	__set_current_state(TASK_RUNNING);
	remove_wait_queue(sk_sleep(sk), &wait);

	/* Cleanup session */
	down_write(&bnep_session_sem);

	/* Delete network device */
	unregister_netdev(dev);

	/* Wakeup user-space polling for socket errors */
	s->sock->sk->sk_err = EUNATCH;

	wake_up_interruptible(sk_sleep(s->sock->sk));

	/* Release the socket */
	fput(s->sock->file);

	__bnep_unlink_session(s);

	up_write(&bnep_session_sem);
	free_netdev(dev);
	module_put_and_exit(0);
	return 0;
}
