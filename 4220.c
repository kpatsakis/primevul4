PJ_DEF(pj_xml_attr*) pj_xml_attr_new( pj_pool_t *pool, const pj_str_t *name,
				      const pj_str_t *value)
{
    pj_xml_attr *attr = alloc_attr(pool);
    pj_strdup( pool, &attr->name, name);
    pj_strdup( pool, &attr->value, value);
    return attr;
}