validate_xml_verbose(xmlNode * xml_blob)
{
    int fd = 0;
    xmlDoc *doc = NULL;
    xmlNode *xml = NULL;
    gboolean rc = FALSE;
    char *filename = strdup(CRM_STATE_DIR "/cib-invalid.XXXXXX");

    umask(S_IWGRP | S_IWOTH | S_IROTH);
    fd = mkstemp(filename);
    write_xml_fd(xml_blob, filename, fd, FALSE);

    dump_file(filename);

    doc = xmlParseFile(filename);
    xml = xmlDocGetRootElement(doc);
    rc = validate_xml(xml, NULL, FALSE);
    free_xml(xml);

    unlink(filename);
    free(filename);

    return rc;
}
