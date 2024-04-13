static unsigned int fanout_demux_bpf(struct packet_fanout *f,
				     struct sk_buff *skb,
				     unsigned int num)
{
	struct bpf_prog *prog;
	unsigned int ret = 0;

	rcu_read_lock();
	prog = rcu_dereference(f->bpf_prog);
	if (prog)
		ret = bpf_prog_run_clear_cb(prog, skb) % num;
	rcu_read_unlock();

	return ret;
}
