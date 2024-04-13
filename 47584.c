__subtract_xml_object(xmlNode * target, xmlNode * patch)
{
    xmlNode *patch_child = NULL;
    xmlNode *cIter = NULL;
    xmlAttrPtr xIter = NULL;

    char *id = NULL;
    const char *name = NULL;
    const char *value = NULL;

    if (target == NULL || patch == NULL) {
        return;
    }

    if (target->type == XML_COMMENT_NODE) {
        gboolean dummy;

        subtract_xml_comment(target->parent, target, patch, &dummy);
    }

    name = crm_element_name(target);
    CRM_CHECK(name != NULL, return);
    CRM_CHECK(safe_str_eq(crm_element_name(target), crm_element_name(patch)), return);
    CRM_CHECK(safe_str_eq(ID(target), ID(patch)), return);

    /* check for XML_DIFF_MARKER in a child */
    id = crm_element_value_copy(target, XML_ATTR_ID);
    value = crm_element_value(patch, XML_DIFF_MARKER);
    if (value != NULL && strcmp(value, "removed:top") == 0) {
        crm_trace("We are the root of the deletion: %s.id=%s", name, id);
        free_xml(target);
        free(id);
        return;
    }

    for (xIter = crm_first_attr(patch); xIter != NULL; xIter = xIter->next) {
        const char *p_name = (const char *)xIter->name;

        /* Removing and then restoring the id field would change the ordering of properties */
        if (safe_str_neq(p_name, XML_ATTR_ID)) {
            xml_remove_prop(target, p_name);
        }
    }

    /* changes to child objects */
    cIter = __xml_first_child(target);
    while (cIter) {
        xmlNode *target_child = cIter;

        cIter = __xml_next(cIter);

        if (target_child->type == XML_COMMENT_NODE) {
            patch_child = find_xml_comment(patch, target_child);

        } else {
            patch_child = find_entity(patch, crm_element_name(target_child), ID(target_child));
        }

        __subtract_xml_object(target_child, patch_child);
    }
    free(id);
}
