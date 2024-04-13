static int do_tcp_getsockopt(struct sock *sk, int level,
		int optname, char __user *optval, int __user *optlen)
{
	struct inet_connection_sock *icsk = inet_csk(sk);
	struct tcp_sock *tp = tcp_sk(sk);
	struct net *net = sock_net(sk);
	int val, len;

	if (get_user(len, optlen))
		return -EFAULT;

	len = min_t(unsigned int, len, sizeof(int));

	if (len < 0)
		return -EINVAL;

	switch (optname) {
	case TCP_MAXSEG:
		val = tp->mss_cache;
		if (!val && ((1 << sk->sk_state) & (TCPF_CLOSE | TCPF_LISTEN)))
			val = tp->rx_opt.user_mss;
		if (tp->repair)
			val = tp->rx_opt.mss_clamp;
		break;
	case TCP_NODELAY:
		val = !!(tp->nonagle&TCP_NAGLE_OFF);
		break;
	case TCP_CORK:
		val = !!(tp->nonagle&TCP_NAGLE_CORK);
		break;
	case TCP_KEEPIDLE:
		val = keepalive_time_when(tp) / HZ;
		break;
	case TCP_KEEPINTVL:
		val = keepalive_intvl_when(tp) / HZ;
		break;
	case TCP_KEEPCNT:
		val = keepalive_probes(tp);
		break;
	case TCP_SYNCNT:
		val = icsk->icsk_syn_retries ? : net->ipv4.sysctl_tcp_syn_retries;
		break;
	case TCP_LINGER2:
		val = tp->linger2;
		if (val >= 0)
			val = (val ? : net->ipv4.sysctl_tcp_fin_timeout) / HZ;
		break;
	case TCP_DEFER_ACCEPT:
		val = retrans_to_secs(icsk->icsk_accept_queue.rskq_defer_accept,
				      TCP_TIMEOUT_INIT / HZ, TCP_RTO_MAX / HZ);
		break;
	case TCP_WINDOW_CLAMP:
		val = tp->window_clamp;
		break;
	case TCP_INFO: {
		struct tcp_info info;

		if (get_user(len, optlen))
			return -EFAULT;

		tcp_get_info(sk, &info);

		len = min_t(unsigned int, len, sizeof(info));
		if (put_user(len, optlen))
			return -EFAULT;
		if (copy_to_user(optval, &info, len))
			return -EFAULT;
		return 0;
	}
	case TCP_CC_INFO: {
		const struct tcp_congestion_ops *ca_ops;
		union tcp_cc_info info;
		size_t sz = 0;
		int attr;

		if (get_user(len, optlen))
			return -EFAULT;

		ca_ops = icsk->icsk_ca_ops;
		if (ca_ops && ca_ops->get_info)
			sz = ca_ops->get_info(sk, ~0U, &attr, &info);

		len = min_t(unsigned int, len, sz);
		if (put_user(len, optlen))
			return -EFAULT;
		if (copy_to_user(optval, &info, len))
			return -EFAULT;
		return 0;
	}
	case TCP_QUICKACK:
		val = !icsk->icsk_ack.pingpong;
		break;

	case TCP_CONGESTION:
		if (get_user(len, optlen))
			return -EFAULT;
		len = min_t(unsigned int, len, TCP_CA_NAME_MAX);
		if (put_user(len, optlen))
			return -EFAULT;
		if (copy_to_user(optval, icsk->icsk_ca_ops->name, len))
			return -EFAULT;
		return 0;

	case TCP_THIN_LINEAR_TIMEOUTS:
		val = tp->thin_lto;
		break;
	case TCP_THIN_DUPACK:
		val = tp->thin_dupack;
		break;

	case TCP_REPAIR:
		val = tp->repair;
		break;

	case TCP_REPAIR_QUEUE:
		if (tp->repair)
			val = tp->repair_queue;
		else
			return -EINVAL;
		break;

	case TCP_REPAIR_WINDOW: {
		struct tcp_repair_window opt;

		if (get_user(len, optlen))
			return -EFAULT;

		if (len != sizeof(opt))
			return -EINVAL;

		if (!tp->repair)
			return -EPERM;

		opt.snd_wl1	= tp->snd_wl1;
		opt.snd_wnd	= tp->snd_wnd;
		opt.max_window	= tp->max_window;
		opt.rcv_wnd	= tp->rcv_wnd;
		opt.rcv_wup	= tp->rcv_wup;

		if (copy_to_user(optval, &opt, len))
			return -EFAULT;
		return 0;
	}
	case TCP_QUEUE_SEQ:
		if (tp->repair_queue == TCP_SEND_QUEUE)
			val = tp->write_seq;
		else if (tp->repair_queue == TCP_RECV_QUEUE)
			val = tp->rcv_nxt;
		else
			return -EINVAL;
		break;

	case TCP_USER_TIMEOUT:
		val = jiffies_to_msecs(icsk->icsk_user_timeout);
		break;

	case TCP_FASTOPEN:
		val = icsk->icsk_accept_queue.fastopenq.max_qlen;
		break;

	case TCP_TIMESTAMP:
		val = tcp_time_stamp + tp->tsoffset;
		break;
	case TCP_NOTSENT_LOWAT:
		val = tp->notsent_lowat;
		break;
	case TCP_SAVE_SYN:
		val = tp->save_syn;
		break;
	case TCP_SAVED_SYN: {
		if (get_user(len, optlen))
			return -EFAULT;

		lock_sock(sk);
		if (tp->saved_syn) {
			if (len < tp->saved_syn[0]) {
				if (put_user(tp->saved_syn[0], optlen)) {
					release_sock(sk);
					return -EFAULT;
				}
				release_sock(sk);
				return -EINVAL;
			}
			len = tp->saved_syn[0];
			if (put_user(len, optlen)) {
				release_sock(sk);
				return -EFAULT;
			}
			if (copy_to_user(optval, tp->saved_syn + 1, len)) {
				release_sock(sk);
				return -EFAULT;
			}
			tcp_saved_syn_free(tp);
			release_sock(sk);
		} else {
			release_sock(sk);
			len = 0;
			if (put_user(len, optlen))
				return -EFAULT;
		}
		return 0;
	}
	default:
		return -ENOPROTOOPT;
	}

	if (put_user(len, optlen))
		return -EFAULT;
	if (copy_to_user(optval, &val, len))
		return -EFAULT;
	return 0;
}
