static void rawsock_tx_work(struct work_struct *work)
{
	struct sock *sk = to_rawsock_sk(work);
	struct nfc_dev *dev = nfc_rawsock(sk)->dev;
	u32 target_idx = nfc_rawsock(sk)->target_idx;
	struct sk_buff *skb;
	int rc;

	pr_debug("sk=%p target_idx=%u\n", sk, target_idx);

	if (sk->sk_shutdown & SEND_SHUTDOWN) {
		rawsock_write_queue_purge(sk);
		return;
	}

	skb = skb_dequeue(&sk->sk_write_queue);

	sock_hold(sk);
	rc = nfc_data_exchange(dev, target_idx, skb,
			       rawsock_data_exchange_complete, sk);
	if (rc) {
		rawsock_report_error(sk, rc);
		sock_put(sk);
	}
}
