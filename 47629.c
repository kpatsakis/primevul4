crm_xml_dump(xmlNode * data, int options, char **buffer, int *offset, int *max, int depth)
{
#if 0
    if (is_not_set(options, xml_log_option_filtered)) {
        /* Turning this code on also changes the PE tests for some reason
         * (not just newlines).  Figure out why before considering to
         * enable this permanently.
         *
         * It exists to help debug slowness in xmlNodeDump() and
         * potentially if we ever want to go back to it.
         *
         * In theory its a good idea (reuse) but our custom version does
         * better for the filtered case and avoids the final strdup() for
         * everything
         */

        time_t now, next;
        xmlDoc *doc = NULL;
        xmlBuffer *xml_buffer = NULL;

        *buffer = NULL;
        doc = getDocPtr(data);
        /* doc will only be NULL if data is */
        CRM_CHECK(doc != NULL, return);

        now = time(NULL);
        xml_buffer = xmlBufferCreate();
        CRM_ASSERT(xml_buffer != NULL);

        /* The default allocator XML_BUFFER_ALLOC_EXACT does far too many
         * realloc()s and it can take upwards of 18 seconds (yes, seconds)
         * to dump a 28kb tree which XML_BUFFER_ALLOC_DOUBLEIT can do in
         * less than 1 second.
         *
         * We could also use xmlBufferCreateSize() to start with a
         * sane-ish initial size and avoid the first few doubles.
         */
        xmlBufferSetAllocationScheme(xml_buffer, XML_BUFFER_ALLOC_DOUBLEIT);

        *max = xmlNodeDump(xml_buffer, doc, data, 0, (options & xml_log_option_formatted));
        if (*max > 0) {
            *buffer = strdup((char *)xml_buffer->content);
        }

        next = time(NULL);
        if ((now + 1) < next) {
            crm_log_xml_trace(data, "Long time");
            crm_err("xmlNodeDump() -> %dbytes took %ds", *max, next - now);
        }

        xmlBufferFree(xml_buffer);
        return;
    }
#endif

    switch(data->type) {
        case XML_ELEMENT_NODE:
            /* Handle below */
            dump_xml_element(data, options, buffer, offset, max, depth);
            break;
        case XML_TEXT_NODE:
            /* Ignore */
            return;
        case XML_COMMENT_NODE:
            dump_xml_comment(data, options, buffer, offset, max, depth);
            break;
        default:
            crm_warn("Unhandled type: %d", data->type);
            return;

            /*
            XML_ATTRIBUTE_NODE = 2
            XML_CDATA_SECTION_NODE = 4
            XML_ENTITY_REF_NODE = 5
            XML_ENTITY_NODE = 6
            XML_PI_NODE = 7
            XML_DOCUMENT_NODE = 9
            XML_DOCUMENT_TYPE_NODE = 10
            XML_DOCUMENT_FRAG_NODE = 11
            XML_NOTATION_NODE = 12
            XML_HTML_DOCUMENT_NODE = 13
            XML_DTD_NODE = 14
            XML_ELEMENT_DECL = 15
            XML_ATTRIBUTE_DECL = 16
            XML_ENTITY_DECL = 17
            XML_NAMESPACE_DECL = 18
            XML_XINCLUDE_START = 19
            XML_XINCLUDE_END = 20
            XML_DOCB_DOCUMENT_NODE = 21
            */
    }

}
