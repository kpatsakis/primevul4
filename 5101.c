PJ_DEF(void) pj_dns_init_srv_rr( pj_dns_parsed_rr *rec,
				 const pj_str_t *res_name,
				 unsigned dnsclass,
				 unsigned ttl,
				 unsigned prio,
				 unsigned weight,
				 unsigned port,
				 const pj_str_t *target)
{
    pj_bzero(rec, sizeof(*rec));
    rec->name = *res_name;
    rec->type = PJ_DNS_TYPE_SRV;
    rec->dnsclass = (pj_uint16_t) dnsclass;
    rec->ttl = ttl;
    rec->rdata.srv.prio = (pj_uint16_t) prio;
    rec->rdata.srv.weight = (pj_uint16_t) weight;
    rec->rdata.srv.port = (pj_uint16_t) port;
    rec->rdata.srv.target = *target;
}