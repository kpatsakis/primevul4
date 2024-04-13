void sock_tx_timestamp(struct sock *sk, __u8 *tx_flags)
{
	*tx_flags = 0;
	if (sock_flag(sk, SOCK_TIMESTAMPING_TX_HARDWARE))
		*tx_flags |= SKBTX_HW_TSTAMP;
	if (sock_flag(sk, SOCK_TIMESTAMPING_TX_SOFTWARE))
		*tx_flags |= SKBTX_SW_TSTAMP;
	if (sock_flag(sk, SOCK_WIFI_STATUS))
		*tx_flags |= SKBTX_WIFI_STATUS;
}
