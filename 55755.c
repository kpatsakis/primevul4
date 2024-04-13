int tcp_ioctl(struct sock *sk, int cmd, unsigned long arg)
{
	struct tcp_sock *tp = tcp_sk(sk);
	int answ;
	bool slow;

	switch (cmd) {
	case SIOCINQ:
		if (sk->sk_state == TCP_LISTEN)
			return -EINVAL;

		slow = lock_sock_fast(sk);
		answ = tcp_inq(sk);
		unlock_sock_fast(sk, slow);
		break;
	case SIOCATMARK:
		answ = tp->urg_data && tp->urg_seq == tp->copied_seq;
		break;
	case SIOCOUTQ:
		if (sk->sk_state == TCP_LISTEN)
			return -EINVAL;

		if ((1 << sk->sk_state) & (TCPF_SYN_SENT | TCPF_SYN_RECV))
			answ = 0;
		else
			answ = tp->write_seq - tp->snd_una;
		break;
	case SIOCOUTQNSD:
		if (sk->sk_state == TCP_LISTEN)
			return -EINVAL;

		if ((1 << sk->sk_state) & (TCPF_SYN_SENT | TCPF_SYN_RECV))
			answ = 0;
		else
			answ = tp->write_seq - tp->snd_nxt;
		break;
	default:
		return -ENOIOCTLCMD;
	}

	return put_user(answ, (int __user *)arg);
}
