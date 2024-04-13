int udp4_seq_show(struct seq_file *seq, void *v)
{
	seq_setwidth(seq, 127);
	if (v == SEQ_START_TOKEN)
		seq_puts(seq, "  sl  local_address rem_address   st tx_queue "
			   "rx_queue tr tm->when retrnsmt   uid  timeout "
			   "inode ref pointer drops");
	else {
		struct udp_iter_state *state = seq->private;

		udp4_format_sock(v, seq, state->bucket);
	}
	seq_pad(seq, '\n');
	return 0;
}
