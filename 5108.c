PJ_DEF(void) pj_dns_packet_dup(pj_pool_t *pool,
			       const pj_dns_parsed_packet*p,
			       unsigned options,
			       pj_dns_parsed_packet **p_dst)
{
    pj_dns_parsed_packet *dst;
    unsigned nametable_count = 0;
#if PJ_DNS_MAX_NAMES_IN_NAMETABLE
    pj_str_t nametable[PJ_DNS_MAX_NAMES_IN_NAMETABLE];
#else
    pj_str_t *nametable = NULL;
#endif
    unsigned i;

    PJ_ASSERT_ON_FAIL(pool && p && p_dst, return);

    /* Create packet and copy header */
    *p_dst = dst = PJ_POOL_ZALLOC_T(pool, pj_dns_parsed_packet);
    pj_memcpy(&dst->hdr, &p->hdr, sizeof(p->hdr));

    /* Initialize section counts in the target packet to zero.
     * If memory allocation fails during copying process, the target packet
     * should have a correct section counts.
     */
    dst->hdr.qdcount = 0;
    dst->hdr.anscount = 0;
    dst->hdr.nscount = 0;
    dst->hdr.arcount = 0;
	

    /* Copy query section */
    if (p->hdr.qdcount && (options & PJ_DNS_NO_QD)==0) {
	dst->q = (pj_dns_parsed_query*)
		 pj_pool_alloc(pool, p->hdr.qdcount * 
				     sizeof(pj_dns_parsed_query));
	for (i=0; i<p->hdr.qdcount; ++i) {
	    copy_query(pool, &dst->q[i], &p->q[i], 
		       &nametable_count, nametable);
	    ++dst->hdr.qdcount;
	}
    }

    /* Copy answer section */
    if (p->hdr.anscount && (options & PJ_DNS_NO_ANS)==0) {
	dst->ans = (pj_dns_parsed_rr*)
		   pj_pool_alloc(pool, p->hdr.anscount * 
				       sizeof(pj_dns_parsed_rr));
	for (i=0; i<p->hdr.anscount; ++i) {
	    copy_rr(pool, &dst->ans[i], &p->ans[i],
		    &nametable_count, nametable);
	    ++dst->hdr.anscount;
	}
    }

    /* Copy NS section */
    if (p->hdr.nscount && (options & PJ_DNS_NO_NS)==0) {
	dst->ns = (pj_dns_parsed_rr*)
		  pj_pool_alloc(pool, p->hdr.nscount * 
				      sizeof(pj_dns_parsed_rr));
	for (i=0; i<p->hdr.nscount; ++i) {
	    copy_rr(pool, &dst->ns[i], &p->ns[i],
		    &nametable_count, nametable);
	    ++dst->hdr.nscount;
	}
    }

    /* Copy additional info section */
    if (p->hdr.arcount && (options & PJ_DNS_NO_AR)==0) {
	dst->arr = (pj_dns_parsed_rr*)
		   pj_pool_alloc(pool, p->hdr.arcount * 
				       sizeof(pj_dns_parsed_rr));
	for (i=0; i<p->hdr.arcount; ++i) {
	    copy_rr(pool, &dst->arr[i], &p->arr[i],
		    &nametable_count, nametable);
	    ++dst->hdr.arcount;
	}
    }
}