PJ_DEF(const char *) pj_dns_get_type_name(int type)
{
    switch (type) {
    case PJ_DNS_TYPE_A:	    return "A";
    case PJ_DNS_TYPE_AAAA:  return "AAAA";
    case PJ_DNS_TYPE_SRV:   return "SRV";
    case PJ_DNS_TYPE_NS:    return "NS";
    case PJ_DNS_TYPE_CNAME: return "CNAME";
    case PJ_DNS_TYPE_PTR:   return "PTR";
    case PJ_DNS_TYPE_MX:    return "MX";
    case PJ_DNS_TYPE_TXT:   return "TXT";
    case PJ_DNS_TYPE_NAPTR: return "NAPTR";
    }
    return "(Unknown)";
}