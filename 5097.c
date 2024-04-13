PJ_DEF(void) pj_dns_init_aaaa_rr(pj_dns_parsed_rr *rec,
				 const pj_str_t *res_name,
				 unsigned dnsclass,
				 unsigned ttl,
				 const pj_in6_addr *ip_addr)
{
    pj_bzero(rec, sizeof(*rec));
    rec->name = *res_name;
    rec->type = PJ_DNS_TYPE_AAAA;
    rec->dnsclass = (pj_uint16_t) dnsclass;
    rec->ttl = ttl;
    rec->rdata.aaaa.ip_addr = *ip_addr;
}