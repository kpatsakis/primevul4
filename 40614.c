static int pfkey_sendmsg(struct kiocb *kiocb,
			 struct socket *sock, struct msghdr *msg, size_t len)
{
	struct sock *sk = sock->sk;
	struct sk_buff *skb = NULL;
	struct sadb_msg *hdr = NULL;
	int err;

	err = -EOPNOTSUPP;
	if (msg->msg_flags & MSG_OOB)
		goto out;

	err = -EMSGSIZE;
	if ((unsigned int)len > sk->sk_sndbuf - 32)
		goto out;

	err = -ENOBUFS;
	skb = alloc_skb(len, GFP_KERNEL);
	if (skb == NULL)
		goto out;

	err = -EFAULT;
	if (memcpy_fromiovec(skb_put(skb,len), msg->msg_iov, len))
		goto out;

	hdr = pfkey_get_base_msg(skb, &err);
	if (!hdr)
		goto out;

	mutex_lock(&xfrm_cfg_mutex);
	err = pfkey_process(sk, skb, hdr);
	mutex_unlock(&xfrm_cfg_mutex);

out:
	if (err && hdr && pfkey_error(hdr, err, sk) == 0)
		err = 0;
	kfree_skb(skb);

	return err ? : len;
}
