static void __fanout_link(struct sock *sk, struct packet_sock *po)
{
	struct packet_fanout *f = po->fanout;

	spin_lock(&f->lock);
	f->arr[f->num_members] = sk;
	smp_wmb();
	f->num_members++;
	if (f->num_members == 1)
		dev_add_pack(&f->prot_hook);
	spin_unlock(&f->lock);
}
