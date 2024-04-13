static pj_xml_attr *alloc_attr( pj_pool_t *pool )
{
    return PJ_POOL_ZALLOC_T(pool, pj_xml_attr);
}