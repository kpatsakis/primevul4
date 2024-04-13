static void tcp_try_to_open(struct sock *sk, int flag, const int prior_unsacked)
{
	struct tcp_sock *tp = tcp_sk(sk);

	tcp_verify_left_out(tp);

	if (!tcp_any_retrans_done(sk))
		tp->retrans_stamp = 0;

	if (flag & FLAG_ECE)
		tcp_enter_cwr(sk);

	if (inet_csk(sk)->icsk_ca_state != TCP_CA_CWR) {
		tcp_try_keep_open(sk);
	} else {
		tcp_cwnd_reduction(sk, prior_unsacked, 0, flag);
	}
}
