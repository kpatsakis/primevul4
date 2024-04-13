int bt_sock_ioctl(struct socket *sock, unsigned int cmd, unsigned long arg)
{
	struct sock *sk = sock->sk;
	struct sk_buff *skb;
	long amount;
	int err;

	BT_DBG("sk %p cmd %x arg %lx", sk, cmd, arg);

	switch (cmd) {
	case TIOCOUTQ:
		if (sk->sk_state == BT_LISTEN)
			return -EINVAL;

		amount = sk->sk_sndbuf - sk_wmem_alloc_get(sk);
		if (amount < 0)
			amount = 0;
		err = put_user(amount, (int __user *) arg);
		break;

	case TIOCINQ:
		if (sk->sk_state == BT_LISTEN)
			return -EINVAL;

		lock_sock(sk);
		skb = skb_peek(&sk->sk_receive_queue);
		amount = skb ? skb->len : 0;
		release_sock(sk);
		err = put_user(amount, (int __user *) arg);
		break;

	case SIOCGSTAMP:
		err = sock_get_timestamp(sk, (struct timeval __user *) arg);
		break;

	case SIOCGSTAMPNS:
		err = sock_get_timestampns(sk, (struct timespec __user *) arg);
		break;

	default:
		err = -ENOIOCTLCMD;
		break;
	}

	return err;
}
