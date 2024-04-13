PJ_DEF(pj_xml_node*) pj_xml_parse( pj_pool_t *pool, char *msg, pj_size_t len)
{
    pj_xml_node *node = NULL;
    pj_scanner scanner;
    PJ_USE_EXCEPTION;

    if (!msg || !len || !pool)
	return NULL;

    pj_scan_init( &scanner, msg, len, 
		  PJ_SCAN_AUTOSKIP_WS|PJ_SCAN_AUTOSKIP_NEWLINE, 
		  &on_syntax_error);
    PJ_TRY {
	node =  xml_parse_node(pool, &scanner);
    }
    PJ_CATCH_ANY {
	PJ_LOG(4,(THIS_FILE, "Syntax error parsing XML in line %d column %d",
		  scanner.line, pj_scan_get_col(&scanner)));
    }
    PJ_END;
    pj_scan_fini( &scanner );
    return node;
}