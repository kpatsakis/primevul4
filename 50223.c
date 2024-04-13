static void nf_ct_frag6_expire(unsigned long data)
{
	struct frag_queue *fq;
	struct net *net;

	fq = container_of((struct inet_frag_queue *)data, struct frag_queue, q);
	net = container_of(fq->q.net, struct net, nf_frag.frags);

	ip6_expire_frag_queue(net, fq, &nf_frags);
}
