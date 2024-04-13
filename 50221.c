static inline struct frag_queue *fq_find(struct net *net, __be32 id,
					 u32 user, struct in6_addr *src,
					 struct in6_addr *dst, int iif, u8 ecn)
{
	struct inet_frag_queue *q;
	struct ip6_create_arg arg;
	unsigned int hash;

	arg.id = id;
	arg.user = user;
	arg.src = src;
	arg.dst = dst;
	arg.iif = iif;
	arg.ecn = ecn;

	local_bh_disable();
	hash = nf_hash_frag(id, src, dst);

	q = inet_frag_find(&net->nf_frag.frags, &nf_frags, &arg, hash);
	local_bh_enable();
	if (IS_ERR_OR_NULL(q)) {
		inet_frag_maybe_warn_overflow(q, pr_fmt());
		return NULL;
	}
	return container_of(q, struct frag_queue, q);
}
