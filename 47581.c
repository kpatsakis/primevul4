__add_xml_object(xmlNode * parent, xmlNode * target, xmlNode * patch)
{
    xmlNode *patch_child = NULL;
    xmlNode *target_child = NULL;
    xmlAttrPtr xIter = NULL;

    const char *id = NULL;
    const char *name = NULL;
    const char *value = NULL;

    if (patch == NULL) {
        return;
    } else if (parent == NULL && target == NULL) {
        return;
    }

    /* check for XML_DIFF_MARKER in a child */
    value = crm_element_value(patch, XML_DIFF_MARKER);
    if (target == NULL
        && value != NULL
        && strcmp(value, "added:top") == 0) {
        id = ID(patch);
        name = crm_element_name(patch);
        crm_trace("We are the root of the addition: %s.id=%s", name, id);
        add_node_copy(parent, patch);
        return;

    } else if(target == NULL) {
        id = ID(patch);
        name = crm_element_name(patch);
        crm_err("Could not locate: %s.id=%s", name, id);
        return;
    }

    if (target->type == XML_COMMENT_NODE) {
        add_xml_comment(parent, target, patch);
    }

    name = crm_element_name(target);
    CRM_CHECK(name != NULL, return);
    CRM_CHECK(safe_str_eq(crm_element_name(target), crm_element_name(patch)), return);
    CRM_CHECK(safe_str_eq(ID(target), ID(patch)), return);

    for (xIter = crm_first_attr(patch); xIter != NULL; xIter = xIter->next) {
        const char *p_name = (const char *)xIter->name;
        const char *p_value = crm_element_value(patch, p_name);

        xml_remove_prop(target, p_name); /* Preserve the patch order */
        crm_xml_add(target, p_name, p_value);
    }

    /* changes to child objects */
    for (patch_child = __xml_first_child(patch); patch_child != NULL;
         patch_child = __xml_next(patch_child)) {

        if (patch_child->type == XML_COMMENT_NODE) {
            target_child = find_xml_comment(target, patch_child);

        } else {
            target_child = find_entity(target, crm_element_name(patch_child), ID(patch_child));
        }

        __add_xml_object(target, target_child, patch_child);
    }
}
