filename2xml(const char *filename)
{
    xmlNode *xml = NULL;
    xmlDocPtr output = NULL;
    const char *match = NULL;
    xmlParserCtxtPtr ctxt = NULL;
    xmlErrorPtr last_error = NULL;
    static int xml_options = XML_PARSE_NOBLANKS | XML_PARSE_RECOVER;

    /* create a parser context */
    ctxt = xmlNewParserCtxt();
    CRM_CHECK(ctxt != NULL, return NULL);

    /* xmlCtxtUseOptions(ctxt, XML_PARSE_NOBLANKS|XML_PARSE_RECOVER); */

    xmlCtxtResetLastError(ctxt);
    xmlSetGenericErrorFunc(ctxt, crm_xml_err);
    /* initGenericErrorDefaultFunc(crm_xml_err); */

    if (filename) {
        match = strstr(filename, ".bz2");
    }

    if (filename == NULL) {
        /* STDIN_FILENO == fileno(stdin) */
        output = xmlCtxtReadFd(ctxt, STDIN_FILENO, "unknown.xml", NULL, xml_options);

    } else if (match == NULL || match[4] != 0) {
        output = xmlCtxtReadFile(ctxt, filename, NULL, xml_options);

    } else {
        char *input = decompress_file(filename);

        output = xmlCtxtReadDoc(ctxt, (const xmlChar *)input, NULL, NULL, xml_options);
        free(input);
    }

    if (output && (xml = xmlDocGetRootElement(output))) {
        strip_text_nodes(xml);
    }

    last_error = xmlCtxtGetLastError(ctxt);
    if (last_error && last_error->code != XML_ERR_OK) {
        /* crm_abort(__FILE__,__FUNCTION__,__LINE__, "last_error->code != XML_ERR_OK", TRUE, TRUE); */
        /*
         * http://xmlsoft.org/html/libxml-xmlerror.html#xmlErrorLevel
         * http://xmlsoft.org/html/libxml-xmlerror.html#xmlParserErrors
         */
        crm_err("Parsing failed (domain=%d, level=%d, code=%d): %s",
                last_error->domain, last_error->level, last_error->code, last_error->message);

        if (last_error && last_error->code != XML_ERR_OK) {
            crm_err("Couldn't%s parse %s", xml ? " fully" : "", filename);
            if (xml != NULL) {
                crm_log_xml_err(xml, "Partial");
            }
        }
    }

    xmlFreeParserCtxt(ctxt);
    return xml;
}
