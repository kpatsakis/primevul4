static void rawsock_write_queue_purge(struct sock *sk)
{
	pr_debug("sk=%p\n", sk);

	spin_lock_bh(&sk->sk_write_queue.lock);
	__skb_queue_purge(&sk->sk_write_queue);
	nfc_rawsock(sk)->tx_work_scheduled = false;
	spin_unlock_bh(&sk->sk_write_queue.lock);
}
