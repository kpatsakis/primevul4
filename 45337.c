static int pn_ioctl(struct sock *sk, int cmd, unsigned long arg)
{
	struct sk_buff *skb;
	int answ;

	switch (cmd) {
	case SIOCINQ:
		lock_sock(sk);
		skb = skb_peek(&sk->sk_receive_queue);
		answ = skb ? skb->len : 0;
		release_sock(sk);
		return put_user(answ, (int __user *)arg);

	case SIOCPNADDRESOURCE:
	case SIOCPNDELRESOURCE: {
			u32 res;
			if (get_user(res, (u32 __user *)arg))
				return -EFAULT;
			if (res >= 256)
				return -EINVAL;
			if (cmd == SIOCPNADDRESOURCE)
				return pn_sock_bind_res(sk, res);
			else
				return pn_sock_unbind_res(sk, res);
		}
	}

	return -ENOIOCTLCMD;
}
