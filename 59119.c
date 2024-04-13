int ip_recv_error(struct sock *sk, struct msghdr *msg, int len, int *addr_len)
{
	struct sock_exterr_skb *serr;
	struct sk_buff *skb;
	DECLARE_SOCKADDR(struct sockaddr_in *, sin, msg->msg_name);
	struct {
		struct sock_extended_err ee;
		struct sockaddr_in	 offender;
	} errhdr;
	int err;
	int copied;

	WARN_ON_ONCE(sk->sk_family == AF_INET6);

	err = -EAGAIN;
	skb = sock_dequeue_err_skb(sk);
	if (!skb)
		goto out;

	copied = skb->len;
	if (copied > len) {
		msg->msg_flags |= MSG_TRUNC;
		copied = len;
	}
	err = skb_copy_datagram_msg(skb, 0, msg, copied);
	if (unlikely(err)) {
		kfree_skb(skb);
		return err;
	}
	sock_recv_timestamp(msg, sk, skb);

	serr = SKB_EXT_ERR(skb);

	if (sin && ipv4_datagram_support_addr(serr)) {
		sin->sin_family = AF_INET;
		sin->sin_addr.s_addr = *(__be32 *)(skb_network_header(skb) +
						   serr->addr_offset);
		sin->sin_port = serr->port;
		memset(&sin->sin_zero, 0, sizeof(sin->sin_zero));
		*addr_len = sizeof(*sin);
	}

	memcpy(&errhdr.ee, &serr->ee, sizeof(struct sock_extended_err));
	sin = &errhdr.offender;
	memset(sin, 0, sizeof(*sin));

	if (ipv4_datagram_support_cmsg(sk, skb, serr->ee.ee_origin)) {
		sin->sin_family = AF_INET;
		sin->sin_addr.s_addr = ip_hdr(skb)->saddr;
		if (inet_sk(sk)->cmsg_flags)
			ip_cmsg_recv(msg, skb);
	}

	put_cmsg(msg, SOL_IP, IP_RECVERR, sizeof(errhdr), &errhdr);

	/* Now we could try to dump offended packet options */

	msg->msg_flags |= MSG_ERRQUEUE;
	err = copied;

	consume_skb(skb);
out:
	return err;
}
