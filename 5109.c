static void copy_rr(pj_pool_t *pool, pj_dns_parsed_rr *dst,
		    const pj_dns_parsed_rr *src,
		    unsigned *nametable_count,
		    pj_str_t nametable[])
{
    pj_memcpy(dst, src, sizeof(*src));
    apply_name_table(nametable_count, nametable, &src->name, pool, &dst->name);

    if (src->data) {
	dst->data = pj_pool_alloc(pool, src->rdlength);
	pj_memcpy(dst->data, src->data, src->rdlength);
    }

    if (src->type == PJ_DNS_TYPE_SRV) {
	apply_name_table(nametable_count, nametable, &src->rdata.srv.target, 
			 pool, &dst->rdata.srv.target);
    } else if (src->type == PJ_DNS_TYPE_A) {
	dst->rdata.a.ip_addr.s_addr =  src->rdata.a.ip_addr.s_addr;
    } else if (src->type == PJ_DNS_TYPE_AAAA) {
	pj_memcpy(&dst->rdata.aaaa.ip_addr, &src->rdata.aaaa.ip_addr,
		  sizeof(pj_in6_addr));
    } else if (src->type == PJ_DNS_TYPE_CNAME) {
	pj_strdup(pool, &dst->rdata.cname.name, &src->rdata.cname.name);
    } else if (src->type == PJ_DNS_TYPE_NS) {
	pj_strdup(pool, &dst->rdata.ns.name, &src->rdata.ns.name);
    } else if (src->type == PJ_DNS_TYPE_PTR) {
	pj_strdup(pool, &dst->rdata.ptr.name, &src->rdata.ptr.name);
    }
}