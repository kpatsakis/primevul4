static void __fanout_set_data_bpf(struct packet_fanout *f, struct bpf_prog *new)
{
	struct bpf_prog *old;

	spin_lock(&f->lock);
	old = rcu_dereference_protected(f->bpf_prog, lockdep_is_held(&f->lock));
	rcu_assign_pointer(f->bpf_prog, new);
	spin_unlock(&f->lock);

	if (old) {
		synchronize_net();
		bpf_prog_destroy(old);
	}
}
