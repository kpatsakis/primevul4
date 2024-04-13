static void ccid3_hc_tx_get_info(struct sock *sk, struct tcp_info *info)
{
	info->tcpi_rto = ccid3_hc_tx_sk(sk)->tx_t_rto;
	info->tcpi_rtt = ccid3_hc_tx_sk(sk)->tx_rtt;
}
