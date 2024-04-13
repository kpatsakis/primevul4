__xml_find_path(xmlNode *top, const char *key)
{
    xmlNode *target = (xmlNode*)top->doc;
    char *id = malloc(XML_BUFFER_SIZE);
    char *tag = malloc(XML_BUFFER_SIZE);
    char *section = malloc(XML_BUFFER_SIZE);
    char *current = strdup(key);
    char *remainder = malloc(XML_BUFFER_SIZE);
    int rc = 0;

    while(current) {
        rc = sscanf (current, "/%[^/]%s", section, remainder);
        if(rc <= 0) {
            crm_trace("Done");
            break;

        } else if(rc > 2) {
            crm_trace("Aborting on %s", current);
            target = NULL;
            break;

        } else if(tag && section) {
            int f = sscanf (section, "%[^[][@id='%[^']", tag, id);

            switch(f) {
                case 1:
                    target = __first_xml_child_match(target, tag, NULL);
                    break;
                case 2:
                    target = __first_xml_child_match(target, tag, id);
                    break;
                default:
                    crm_trace("Aborting on %s", section);
                    target = NULL;
                    break;
            }

            if(rc == 1 || target == NULL) {
                crm_trace("Done");
                break;

            } else {
                char *tmp = current;
                current = remainder;
                remainder = tmp;
            }
        }
    }

    if(target) {
        char *path = (char *)xmlGetNodePath(target);

        crm_trace("Found %s for %s", path, key);
        free(path);
    } else {
        crm_debug("No match for %s", key);
    }

    free(remainder);
    free(current);
    free(section);
    free(tag);
    free(id);
    return target;
}
