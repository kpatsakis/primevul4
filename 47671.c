save_xml_to_file(xmlNode * xml, const char *desc, const char *filename)
{
    char *f = NULL;

    if (filename == NULL) {
        char *uuid = crm_generate_uuid();

        f = crm_strdup_printf("/tmp/%s", uuid);
        filename = f;
        free(uuid);
    }

    crm_info("Saving %s to %s", desc, filename);
    write_xml_file(xml, filename, FALSE);
    free(f);
}
