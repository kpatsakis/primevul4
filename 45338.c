static int pn_sendmsg(struct kiocb *iocb, struct sock *sk,
			struct msghdr *msg, size_t len)
{
	struct sockaddr_pn *target;
	struct sk_buff *skb;
	int err;

	if (msg->msg_flags & ~(MSG_DONTWAIT|MSG_EOR|MSG_NOSIGNAL|
				MSG_CMSG_COMPAT))
		return -EOPNOTSUPP;

	if (msg->msg_name == NULL)
		return -EDESTADDRREQ;

	if (msg->msg_namelen < sizeof(struct sockaddr_pn))
		return -EINVAL;

	target = (struct sockaddr_pn *)msg->msg_name;
	if (target->spn_family != AF_PHONET)
		return -EAFNOSUPPORT;

	skb = sock_alloc_send_skb(sk, MAX_PHONET_HEADER + len,
					msg->msg_flags & MSG_DONTWAIT, &err);
	if (skb == NULL)
		return err;
	skb_reserve(skb, MAX_PHONET_HEADER);

	err = memcpy_fromiovec((void *)skb_put(skb, len), msg->msg_iov, len);
	if (err < 0) {
		kfree_skb(skb);
		return err;
	}

	/*
	 * Fill in the Phonet header and
	 * finally pass the packet forwards.
	 */
	err = pn_skb_send(sk, skb, target);

	/* If ok, return len. */
	return (err >= 0) ? len : err;
}
