subtract_xml_object(xmlNode * parent, xmlNode * left, xmlNode * right,
                    gboolean full, gboolean * changed, const char *marker)
{
    gboolean dummy = FALSE;
    gboolean skip = FALSE;
    xmlNode *diff = NULL;
    xmlNode *right_child = NULL;
    xmlNode *left_child = NULL;
    xmlAttrPtr xIter = NULL;

    const char *id = NULL;
    const char *name = NULL;
    const char *value = NULL;
    const char *right_val = NULL;

    int lpc = 0;
    static int filter_len = DIMOF(filter);

    if (changed == NULL) {
        changed = &dummy;
    }

    if (left == NULL) {
        return NULL;
    }

    if (left->type == XML_COMMENT_NODE) {
        return subtract_xml_comment(parent, left, right, changed);
    }

    id = ID(left);
    if (right == NULL) {
        xmlNode *deleted = NULL;

        crm_trace("Processing <%s id=%s> (complete copy)", crm_element_name(left), id);
        deleted = add_node_copy(parent, left);
        crm_xml_add(deleted, XML_DIFF_MARKER, marker);

        *changed = TRUE;
        return deleted;
    }

    name = crm_element_name(left);
    CRM_CHECK(name != NULL, return NULL);
    CRM_CHECK(safe_str_eq(crm_element_name(left), crm_element_name(right)), return NULL);

    /* check for XML_DIFF_MARKER in a child */
    value = crm_element_value(right, XML_DIFF_MARKER);
    if (value != NULL && strcmp(value, "removed:top") == 0) {
        crm_trace("We are the root of the deletion: %s.id=%s", name, id);
        *changed = TRUE;
        return NULL;
    }

    /* Avoiding creating the full heirarchy would save even more work here */
    diff = create_xml_node(parent, name);

    /* Reset filter */
    for (lpc = 0; lpc < filter_len; lpc++) {
        filter[lpc].found = FALSE;
    }

    /* changes to child objects */
    for (left_child = __xml_first_child(left); left_child != NULL;
         left_child = __xml_next(left_child)) {
        gboolean child_changed = FALSE;

        if (left_child->type == XML_COMMENT_NODE) {
            right_child = find_xml_comment(right, left_child);

        } else {
            right_child = find_entity(right, crm_element_name(left_child), ID(left_child));
        }

        subtract_xml_object(diff, left_child, right_child, full, &child_changed, marker);
        if (child_changed) {
            *changed = TRUE;
        }
    }

    if (*changed == FALSE) {
        /* Nothing to do */

    } else if (full) {
        xmlAttrPtr pIter = NULL;

        for (pIter = crm_first_attr(left); pIter != NULL; pIter = pIter->next) {
            const char *p_name = (const char *)pIter->name;
            const char *p_value = crm_attr_value(pIter);

            xmlSetProp(diff, (const xmlChar *)p_name, (const xmlChar *)p_value);
        }

        /* We already have everything we need... */
        goto done;

    } else if (id) {
        xmlSetProp(diff, (const xmlChar *)XML_ATTR_ID, (const xmlChar *)id);
    }

    /* changes to name/value pairs */
    for (xIter = crm_first_attr(left); xIter != NULL; xIter = xIter->next) {
        const char *prop_name = (const char *)xIter->name;

        if (strcmp(prop_name, XML_ATTR_ID) == 0) {
            continue;
        }

        skip = FALSE;
        for (lpc = 0; skip == FALSE && lpc < filter_len; lpc++) {
            if (filter[lpc].found == FALSE && strcmp(prop_name, filter[lpc].string) == 0) {
                filter[lpc].found = TRUE;
                skip = TRUE;
                break;
            }
        }

        if (skip) {
            continue;
        }

        right_val = crm_element_value(right, prop_name);
        if (right_val == NULL) {
            /* new */
            *changed = TRUE;
            if (full) {
                xmlAttrPtr pIter = NULL;

                for (pIter = crm_first_attr(left); pIter != NULL; pIter = pIter->next) {
                    const char *p_name = (const char *)pIter->name;
                    const char *p_value = crm_attr_value(pIter);

                    xmlSetProp(diff, (const xmlChar *)p_name, (const xmlChar *)p_value);
                }
                break;

            } else {
                const char *left_value = crm_element_value(left, prop_name);

                xmlSetProp(diff, (const xmlChar *)prop_name, (const xmlChar *)value);
                crm_xml_add(diff, prop_name, left_value);
            }

        } else {
            /* Only now do we need the left value */
            const char *left_value = crm_element_value(left, prop_name);

            if (strcmp(left_value, right_val) == 0) {
                /* unchanged */

            } else {
                *changed = TRUE;
                if (full) {
                    xmlAttrPtr pIter = NULL;

                    crm_trace("Changes detected to %s in <%s id=%s>", prop_name,
                              crm_element_name(left), id);
                    for (pIter = crm_first_attr(left); pIter != NULL; pIter = pIter->next) {
                        const char *p_name = (const char *)pIter->name;
                        const char *p_value = crm_attr_value(pIter);

                        xmlSetProp(diff, (const xmlChar *)p_name, (const xmlChar *)p_value);
                    }
                    break;

                } else {
                    crm_trace("Changes detected to %s (%s -> %s) in <%s id=%s>",
                              prop_name, left_value, right_val, crm_element_name(left), id);
                    crm_xml_add(diff, prop_name, left_value);
                }
            }
        }
    }

    if (*changed == FALSE) {
        free_xml(diff);
        return NULL;

    } else if (full == FALSE && id) {
        crm_xml_add(diff, XML_ATTR_ID, id);
    }
  done:
    return diff;
}
