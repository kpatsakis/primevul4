PJ_DEF(pj_status_t) pj_dns_parse_packet( pj_pool_t *pool,
				  	 const void *packet,
					 unsigned size,
					 pj_dns_parsed_packet **p_res)
{
    pj_dns_parsed_packet *res;
    const pj_uint8_t *start, *end;
    pj_status_t status;
    unsigned i;

    /* Sanity checks */
    PJ_ASSERT_RETURN(pool && packet && size && p_res, PJ_EINVAL);

    /* Packet size must be at least as big as the header */
    if (size < sizeof(pj_dns_hdr))
	return PJLIB_UTIL_EDNSINSIZE;

    /* Create the structure */
    res = PJ_POOL_ZALLOC_T(pool, pj_dns_parsed_packet);

    /* Copy the DNS header, and convert endianness to host byte order */
    pj_memcpy(&res->hdr, packet, sizeof(pj_dns_hdr));
    res->hdr.id	      = pj_ntohs(res->hdr.id);
    res->hdr.flags    = pj_ntohs(res->hdr.flags);
    res->hdr.qdcount  = pj_ntohs(res->hdr.qdcount);
    res->hdr.anscount = pj_ntohs(res->hdr.anscount);
    res->hdr.nscount  = pj_ntohs(res->hdr.nscount);
    res->hdr.arcount  = pj_ntohs(res->hdr.arcount);

    /* Mark start and end of payload */
    start = ((const pj_uint8_t*)packet) + sizeof(pj_dns_hdr);
    end = ((const pj_uint8_t*)packet) + size;

    /* Parse query records (if any).
     */
    if (res->hdr.qdcount) {
	res->q = (pj_dns_parsed_query*)
		 pj_pool_zalloc(pool, res->hdr.qdcount *
				      sizeof(pj_dns_parsed_query));
	for (i=0; i<res->hdr.qdcount; ++i) {
	    int parsed_len = 0;
	    
	    status = parse_query(&res->q[i], pool, (const pj_uint8_t*)packet, 
	    			 start, end, &parsed_len);
	    if (status != PJ_SUCCESS)
		return status;

	    start += parsed_len;
	}
    }

    /* Parse answer, if any */
    if (res->hdr.anscount) {
	res->ans = (pj_dns_parsed_rr*)
		   pj_pool_zalloc(pool, res->hdr.anscount * 
					sizeof(pj_dns_parsed_rr));

	for (i=0; i<res->hdr.anscount; ++i) {
	    int parsed_len;

	    status = parse_rr(&res->ans[i], pool, (const pj_uint8_t*)packet, 
	    		      start, end, &parsed_len);
	    if (status != PJ_SUCCESS)
		return status;

	    start += parsed_len;
	}
    }

    /* Parse authoritative NS records, if any */
    if (res->hdr.nscount) {
	res->ns = (pj_dns_parsed_rr*)
		  pj_pool_zalloc(pool, res->hdr.nscount *
				       sizeof(pj_dns_parsed_rr));

	for (i=0; i<res->hdr.nscount; ++i) {
	    int parsed_len;

	    status = parse_rr(&res->ns[i], pool, (const pj_uint8_t*)packet, 
	    		      start, end, &parsed_len);
	    if (status != PJ_SUCCESS)
		return status;

	    start += parsed_len;
	}
    }

    /* Parse additional RR answer, if any */
    if (res->hdr.arcount) {
	res->arr = (pj_dns_parsed_rr*)
		   pj_pool_zalloc(pool, res->hdr.arcount *
					sizeof(pj_dns_parsed_rr));

	for (i=0; i<res->hdr.arcount; ++i) {
	    int parsed_len;

	    status = parse_rr(&res->arr[i], pool, (const pj_uint8_t*)packet, 
	    		      start, end, &parsed_len);
	    if (status != PJ_SUCCESS)
		return status;

	    start += parsed_len;
	}
    }

    /* Looks like everything is okay */
    *p_res = res;

    return PJ_SUCCESS;
}