apply_transformation(xmlNode * xml, const char *transform)
{
    char *xform = NULL;
    xmlNode *out = NULL;
    xmlDocPtr res = NULL;
    xmlDocPtr doc = NULL;
    xsltStylesheet *xslt = NULL;

    CRM_CHECK(xml != NULL, return FALSE);
    doc = getDocPtr(xml);
    xform = get_schema_path(NULL, transform);

    xmlLoadExtDtdDefaultValue = 1;
    xmlSubstituteEntitiesDefault(1);

    xslt = xsltParseStylesheetFile((const xmlChar *)xform);
    CRM_CHECK(xslt != NULL, goto cleanup);

    res = xsltApplyStylesheet(xslt, doc, NULL);
    CRM_CHECK(res != NULL, goto cleanup);

    out = xmlDocGetRootElement(res);

  cleanup:
    if (xslt) {
        xsltFreeStylesheet(xslt);
    }

    free(xform);

    return out;
}
