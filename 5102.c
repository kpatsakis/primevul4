static void apply_name_table( unsigned *count,
			      pj_str_t nametable[],
		    	      const pj_str_t *src,
			      pj_pool_t *pool,
			      pj_str_t *dst)
{
    unsigned i;

    /* Scan strings in nametable */
    for (i=0; i<*count; ++i) {
	if (pj_stricmp(&nametable[i], src) == 0)
	    break;
    }

    /* If name is found in nametable, use the pointer in the nametable */
    if (i != *count) {
	dst->ptr = nametable[i].ptr;
	dst->slen = nametable[i].slen;
	return;
    }

    /* Otherwise duplicate the string, and insert new name in nametable */
    pj_strdup(pool, dst, src);

    if (*count < PJ_DNS_MAX_NAMES_IN_NAMETABLE) {
	nametable[*count].ptr = dst->ptr;
	nametable[*count].slen = dst->slen;

	++(*count);
    }
}