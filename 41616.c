static struct sctp_bind_bucket *sctp_bucket_create(
	struct sctp_bind_hashbucket *head, struct net *net, unsigned short snum)
{
	struct sctp_bind_bucket *pp;

	pp = kmem_cache_alloc(sctp_bucket_cachep, GFP_ATOMIC);
	if (pp) {
		SCTP_DBG_OBJCNT_INC(bind_bucket);
		pp->port = snum;
		pp->fastreuse = 0;
		INIT_HLIST_HEAD(&pp->owner);
		pp->net = net;
		hlist_add_head(&pp->node, &head->chain);
	}
	return pp;
}
