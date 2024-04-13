static int tcp6_seq_show(struct seq_file *seq, void *v)
{
	struct tcp_iter_state *st;
	struct sock *sk = v;

	if (v == SEQ_START_TOKEN) {
		seq_puts(seq,
			 "  sl  "
			 "local_address                         "
			 "remote_address                        "
			 "st tx_queue rx_queue tr tm->when retrnsmt"
			 "   uid  timeout inode\n");
		goto out;
	}
	st = seq->private;

	if (sk->sk_state == TCP_TIME_WAIT)
		get_timewait6_sock(seq, v, st->num);
	else if (sk->sk_state == TCP_NEW_SYN_RECV)
		get_openreq6(seq, v, st->num);
	else
		get_tcp6_sock(seq, v, st->num);
out:
	return 0;
}
