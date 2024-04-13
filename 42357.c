static int ccid3_hc_rx_getsockopt(struct sock *sk, const int optname, int len,
				  u32 __user *optval, int __user *optlen)
{
	const struct ccid3_hc_rx_sock *hc = ccid3_hc_rx_sk(sk);
	struct tfrc_rx_info rx_info;
	const void *val;

	switch (optname) {
	case DCCP_SOCKOPT_CCID_RX_INFO:
		if (len < sizeof(rx_info))
			return -EINVAL;
		rx_info.tfrcrx_x_recv = hc->rx_x_recv;
		rx_info.tfrcrx_rtt    = hc->rx_rtt;
		rx_info.tfrcrx_p      = tfrc_invert_loss_event_rate(hc->rx_pinv);
		len = sizeof(rx_info);
		val = &rx_info;
		break;
	default:
		return -ENOPROTOOPT;
	}

	if (put_user(len, optlen) || copy_to_user(optval, val, len))
		return -EFAULT;

	return 0;
}
