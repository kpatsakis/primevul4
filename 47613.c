copy_xml(xmlNode * src)
{
    xmlDoc *doc = xmlNewDoc((const xmlChar *)"1.0");
    xmlNode *copy = xmlDocCopyNode(src, doc, 1);

    xmlDocSetRootElement(doc, copy);
    xmlSetTreeDoc(copy, doc);
    return copy;
}
