static int rawsock_sendmsg(struct kiocb *iocb, struct socket *sock,
			   struct msghdr *msg, size_t len)
{
	struct sock *sk = sock->sk;
	struct nfc_dev *dev = nfc_rawsock(sk)->dev;
	struct sk_buff *skb;
	int rc;

	pr_debug("sock=%p sk=%p len=%zu\n", sock, sk, len);

	if (msg->msg_namelen)
		return -EOPNOTSUPP;

	if (sock->state != SS_CONNECTED)
		return -ENOTCONN;

	skb = nfc_alloc_send_skb(dev, sk, msg->msg_flags, len, &rc);
	if (skb == NULL)
		return rc;

	rc = memcpy_fromiovec(skb_put(skb, len), msg->msg_iov, len);
	if (rc < 0) {
		kfree_skb(skb);
		return rc;
	}

	spin_lock_bh(&sk->sk_write_queue.lock);
	__skb_queue_tail(&sk->sk_write_queue, skb);
	if (!nfc_rawsock(sk)->tx_work_scheduled) {
		schedule_work(&nfc_rawsock(sk)->tx_work);
		nfc_rawsock(sk)->tx_work_scheduled = true;
	}
	spin_unlock_bh(&sk->sk_write_queue.lock);

	return len;
}
