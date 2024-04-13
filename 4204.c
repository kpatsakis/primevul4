PJ_DEF(int) pj_xml_print(const pj_xml_node *node, char *buf, pj_size_t len,
			 pj_bool_t include_prolog)
{
    int prolog_len = 0;
    int printed;

    if (!node || !buf || !len)
	return 0;

    if (include_prolog) {
	pj_str_t prolog = {"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n", 39};
	if ((int)len < prolog.slen)
	    return -1;
	pj_memcpy(buf, prolog.ptr, prolog.slen);
	prolog_len = (int)prolog.slen;
    }

    printed = xml_print_node(node, 0, buf+prolog_len, len-prolog_len) + prolog_len;
    if (printed > 0 && len-printed >= 1) {
	buf[printed++] = '\n';
    }
    return printed;
}