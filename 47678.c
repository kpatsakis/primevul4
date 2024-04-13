string2xml(const char *input)
{
    xmlNode *xml = NULL;
    xmlDocPtr output = NULL;
    xmlParserCtxtPtr ctxt = NULL;
    xmlErrorPtr last_error = NULL;

    if (input == NULL) {
        crm_err("Can't parse NULL input");
        return NULL;
    }

    /* create a parser context */
    ctxt = xmlNewParserCtxt();
    CRM_CHECK(ctxt != NULL, return NULL);

    /* xmlCtxtUseOptions(ctxt, XML_PARSE_NOBLANKS|XML_PARSE_RECOVER); */

    xmlCtxtResetLastError(ctxt);
    xmlSetGenericErrorFunc(ctxt, crm_xml_err);
    /* initGenericErrorDefaultFunc(crm_xml_err); */
    output =
        xmlCtxtReadDoc(ctxt, (const xmlChar *)input, NULL, NULL,
                       XML_PARSE_NOBLANKS | XML_PARSE_RECOVER);
    if (output) {
        xml = xmlDocGetRootElement(output);
    }
    last_error = xmlCtxtGetLastError(ctxt);
    if (last_error && last_error->code != XML_ERR_OK) {
        /* crm_abort(__FILE__,__FUNCTION__,__LINE__, "last_error->code != XML_ERR_OK", TRUE, TRUE); */
        /*
         * http://xmlsoft.org/html/libxml-xmlerror.html#xmlErrorLevel
         * http://xmlsoft.org/html/libxml-xmlerror.html#xmlParserErrors
         */
        crm_warn("Parsing failed (domain=%d, level=%d, code=%d): %s",
                 last_error->domain, last_error->level, last_error->code, last_error->message);

        if (last_error->code == XML_ERR_DOCUMENT_EMPTY) {
            CRM_LOG_ASSERT("Cannot parse an empty string");

        } else if (last_error->code != XML_ERR_DOCUMENT_END) {
            crm_err("Couldn't%s parse %d chars: %s", xml ? " fully" : "", (int)strlen(input),
                    input);
            if (xml != NULL) {
                crm_log_xml_err(xml, "Partial");
            }

        } else {
            int len = strlen(input);
            int lpc = 0;

            while(lpc < len) {
                crm_warn("Parse error[+%.3d]: %.80s", lpc, input+lpc);
                lpc += 80;
            }

            CRM_LOG_ASSERT("String parsing error");
        }
    }

    xmlFreeParserCtxt(ctxt);
    return xml;
}
