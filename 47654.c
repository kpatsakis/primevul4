getDocPtr(xmlNode * node)
{
    xmlDoc *doc = NULL;

    CRM_CHECK(node != NULL, return NULL);

    doc = node->doc;
    if (doc == NULL) {
        doc = xmlNewDoc((const xmlChar *)"1.0");
        xmlDocSetRootElement(doc, node);
        xmlSetTreeDoc(node, doc);
    }
    return doc;
}
