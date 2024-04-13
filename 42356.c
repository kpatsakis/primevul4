static void ccid3_hc_rx_get_info(struct sock *sk, struct tcp_info *info)
{
	info->tcpi_ca_state = ccid3_hc_rx_sk(sk)->rx_state;
	info->tcpi_options  |= TCPI_OPT_TIMESTAMPS;
	info->tcpi_rcv_rtt  = ccid3_hc_rx_sk(sk)->rx_rtt;
}
