static void copy_query(pj_pool_t *pool, pj_dns_parsed_query *dst,
		       const pj_dns_parsed_query *src,
		       unsigned *nametable_count,
		       pj_str_t nametable[])
{
    pj_memcpy(dst, src, sizeof(*src));
    apply_name_table(nametable_count, nametable, &src->name, pool, &dst->name);
}