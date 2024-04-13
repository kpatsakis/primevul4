xml_apply_patchset_v2(xmlNode *xml, xmlNode *patchset, bool check_version) 
{
    int rc = pcmk_ok;
    xmlNode *change = NULL;
    for (change = __xml_first_child(patchset); change != NULL; change = __xml_next(change)) {
        xmlNode *match = NULL;
        const char *op = crm_element_value(change, XML_DIFF_OP);
        const char *xpath = crm_element_value(change, XML_DIFF_PATH);

        crm_trace("Processing %s %s", change->name, op);
        if(op == NULL) {
            continue;
        }

#if 0
        match = get_xpath_object(xpath, xml, LOG_TRACE);
#else
        match = __xml_find_path(xml, xpath);
#endif
        crm_trace("Performing %s on %s with %p", op, xpath, match);

        if(match == NULL && strcmp(op, "delete") == 0) {
            crm_debug("No %s match for %s in %p", op, xpath, xml->doc);
            continue;

        } else if(match == NULL) {
            crm_err("No %s match for %s in %p", op, xpath, xml->doc);
            rc = -pcmk_err_diff_failed;
            continue;

        } else if(strcmp(op, "create") == 0) {
            int position = 0;
            xmlNode *child = NULL;
            xmlNode *match_child = NULL;

            match_child = match->children;
            crm_element_value_int(change, XML_DIFF_POSITION, &position);

            while(match_child && position != __xml_offset(match_child)) {
                match_child = match_child->next;
            }

            child = xmlDocCopyNode(change->children, match->doc, 1);
            if(match_child) {
                crm_trace("Adding %s at position %d", child->name, position);
                xmlAddPrevSibling(match_child, child);

            } else if(match->last) { /* Add to the end */
                crm_trace("Adding %s at position %d (end)", child->name, position);
                xmlAddNextSibling(match->last, child);

            } else {
                crm_trace("Adding %s at position %d (first)", child->name, position);
                CRM_LOG_ASSERT(position == 0);
                xmlAddChild(match, child);
            }
            crm_node_created(child);

        } else if(strcmp(op, "move") == 0) {
            int position = 0;

            crm_element_value_int(change, XML_DIFF_POSITION, &position);
            if(position != __xml_offset(match)) {
                xmlNode *match_child = NULL;
                int p = position;

                if(p > __xml_offset(match)) {
                    p++; /* Skip ourselves */
                }

                CRM_ASSERT(match->parent != NULL);
                match_child = match->parent->children;

                while(match_child && p != __xml_offset(match_child)) {
                    match_child = match_child->next;
                }

                crm_trace("Moving %s to position %d (was %d, prev %p, %s %p)",
                         match->name, position, __xml_offset(match), match->prev,
                         match_child?"next":"last", match_child?match_child:match->parent->last);

                if(match_child) {
                    xmlAddPrevSibling(match_child, match);

                } else {
                    CRM_ASSERT(match->parent->last != NULL);
                    xmlAddNextSibling(match->parent->last, match);
                }

            } else {
                crm_trace("%s is already in position %d", match->name, position);
            }

            if(position != __xml_offset(match)) {
                crm_err("Moved %s.%d to position %d instead of %d (%p)",
                        match->name, ID(match), __xml_offset(match), position, match->prev);
                rc = -pcmk_err_diff_failed;
            }

        } else if(strcmp(op, "delete") == 0) {
            free_xml(match);

        } else if(strcmp(op, "modify") == 0) {
            xmlAttr *pIter = crm_first_attr(match);
            xmlNode *attrs = __xml_first_child(first_named_child(change, XML_DIFF_RESULT));

            if(attrs == NULL) {
                rc = -ENOMSG;
                continue;
            }
            while(pIter != NULL) {
                const char *name = (const char *)pIter->name;

                pIter = pIter->next;
                xml_remove_prop(match, name);
            }

            for (pIter = crm_first_attr(attrs); pIter != NULL; pIter = pIter->next) {
                const char *name = (const char *)pIter->name;
                const char *value = crm_element_value(attrs, name);

                crm_xml_add(match, name, value);
            }

        } else {
            crm_err("Unknown operation: %s", op);
        }
    }
    return rc;
}
