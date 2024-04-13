validate_with_dtd(xmlDocPtr doc, gboolean to_logs, const char *dtd_file)
{
    gboolean valid = TRUE;

    xmlDtdPtr dtd = NULL;
    xmlValidCtxtPtr cvp = NULL;

    CRM_CHECK(doc != NULL, return FALSE);
    CRM_CHECK(dtd_file != NULL, return FALSE);

    dtd = xmlParseDTD(NULL, (const xmlChar *)dtd_file);
    if(dtd == NULL) {
        crm_err("Could not locate/parse DTD: %s", dtd_file);
        return TRUE;
    }

    cvp = xmlNewValidCtxt();
    if(cvp) {
        if (to_logs) {
            cvp->userData = (void *)LOG_ERR;
            cvp->error = (xmlValidityErrorFunc) xml_log;
            cvp->warning = (xmlValidityWarningFunc) xml_log;
        } else {
            cvp->userData = (void *)stderr;
            cvp->error = (xmlValidityErrorFunc) fprintf;
            cvp->warning = (xmlValidityWarningFunc) fprintf;
        }

        if (!xmlValidateDtd(cvp, doc, dtd)) {
            valid = FALSE;
        }
        xmlFreeValidCtxt(cvp);

    } else {
        crm_err("Internal error: No valid context");
    }

    xmlFreeDtd(dtd);
    return valid;
}
