static void rawsock_data_exchange_complete(void *context, struct sk_buff *skb,
					   int err)
{
	struct sock *sk = (struct sock *) context;

	BUG_ON(in_irq());

	pr_debug("sk=%p err=%d\n", sk, err);

	if (err)
		goto error;

	err = rawsock_add_header(skb);
	if (err)
		goto error_skb;

	err = sock_queue_rcv_skb(sk, skb);
	if (err)
		goto error_skb;

	spin_lock_bh(&sk->sk_write_queue.lock);
	if (!skb_queue_empty(&sk->sk_write_queue))
		schedule_work(&nfc_rawsock(sk)->tx_work);
	else
		nfc_rawsock(sk)->tx_work_scheduled = false;
	spin_unlock_bh(&sk->sk_write_queue.lock);

	sock_put(sk);
	return;

error_skb:
	kfree_skb(skb);

error:
	rawsock_report_error(sk, err);
	sock_put(sk);
}
