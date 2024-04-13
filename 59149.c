static int tcp_repair_options_est(struct tcp_sock *tp,
		struct tcp_repair_opt __user *optbuf, unsigned int len)
{
	struct tcp_repair_opt opt;

	while (len >= sizeof(opt)) {
		if (copy_from_user(&opt, optbuf, sizeof(opt)))
			return -EFAULT;

		optbuf++;
		len -= sizeof(opt);

		switch (opt.opt_code) {
		case TCPOPT_MSS:
			tp->rx_opt.mss_clamp = opt.opt_val;
			break;
		case TCPOPT_WINDOW:
			{
				u16 snd_wscale = opt.opt_val & 0xFFFF;
				u16 rcv_wscale = opt.opt_val >> 16;

				if (snd_wscale > 14 || rcv_wscale > 14)
					return -EFBIG;

				tp->rx_opt.snd_wscale = snd_wscale;
				tp->rx_opt.rcv_wscale = rcv_wscale;
				tp->rx_opt.wscale_ok = 1;
			}
			break;
		case TCPOPT_SACK_PERM:
			if (opt.opt_val != 0)
				return -EINVAL;

			tp->rx_opt.sack_ok |= TCP_SACK_SEEN;
			if (sysctl_tcp_fack)
				tcp_enable_fack(tp);
			break;
		case TCPOPT_TIMESTAMP:
			if (opt.opt_val != 0)
				return -EINVAL;

			tp->rx_opt.tstamp_ok = 1;
			break;
		}
	}

	return 0;
}
