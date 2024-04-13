static struct sctp_bind_bucket *sctp_bucket_create(
	struct sctp_bind_hashbucket *head, unsigned short snum)
{
	struct sctp_bind_bucket *pp;

	pp = kmem_cache_alloc(sctp_bucket_cachep, GFP_ATOMIC);
	SCTP_DBG_OBJCNT_INC(bind_bucket);
	if (pp) {
		pp->port = snum;
		pp->fastreuse = 0;
		INIT_HLIST_HEAD(&pp->owner);
		if ((pp->next = head->chain) != NULL)
			pp->next->pprev = &pp->next;
		head->chain = pp;
		pp->pprev = &head->chain;
	}
	return pp;
}
