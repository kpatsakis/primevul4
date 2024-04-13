__xml_diff_object(xmlNode * old, xmlNode * new)
{
    xmlNode *cIter = NULL;
    xmlAttr *pIter = NULL;

    CRM_CHECK(new != NULL, return);
    if(old == NULL) {
        crm_node_created(new);
        __xml_acl_post_process(new); /* Check creation is allowed */
        return;

    } else {
        xml_private_t *p = new->_private;

        if(p->flags & xpf_processed) {
            /* Avoid re-comparing nodes */
            return;
        }
        p->flags |= xpf_processed;
    }

    for (pIter = crm_first_attr(new); pIter != NULL; pIter = pIter->next) {
        xml_private_t *p = pIter->_private;

        /* Assume everything was just created and take it from there */
        p->flags |= xpf_created;
    }

    for (pIter = crm_first_attr(old); pIter != NULL; ) {
        xmlAttr *prop = pIter;
        xml_private_t *p = NULL;
        const char *name = (const char *)pIter->name;
        const char *old_value = crm_element_value(old, name);
        xmlAttr *exists = xmlHasProp(new, pIter->name);

        pIter = pIter->next;
        if(exists == NULL) {
            p = new->doc->_private;

            /* Prevent the dirty flag being set recursively upwards */
            clear_bit(p->flags, xpf_tracking);
            exists = xmlSetProp(new, (const xmlChar *)name, (const xmlChar *)old_value);
            set_bit(p->flags, xpf_tracking);

            p = exists->_private;
            p->flags = 0;

            crm_trace("Lost %s@%s=%s", old->name, name, old_value);
            xml_remove_prop(new, name);

        } else {
            int p_new = __xml_offset((xmlNode*)exists);
            int p_old = __xml_offset((xmlNode*)prop);
            const char *value = crm_element_value(new, name);

            p = exists->_private;
            p->flags = (p->flags & ~xpf_created);

            if(strcmp(value, old_value) != 0) {
                /* Restore the original value, so we can call crm_xml_add() whcih checks ACLs */
                char *vcopy = crm_element_value_copy(new, name);

                crm_trace("Modified %s@%s %s->%s", old->name, name, old_value, vcopy);
                xmlSetProp(new, prop->name, (const xmlChar *)old_value);
                crm_xml_add(new, name, vcopy);
                free(vcopy);

            } else if(p_old != p_new) {
                crm_info("Moved %s@%s (%d -> %d)", old->name, name, p_old, p_new);
                __xml_node_dirty(new);
                p->flags |= xpf_dirty|xpf_moved;

                if(p_old > p_new) {
                    p = prop->_private;
                    p->flags |= xpf_skip;

                } else {
                    p = exists->_private;
                    p->flags |= xpf_skip;
                }
            }
        }
    }

    for (pIter = crm_first_attr(new); pIter != NULL; ) {
        xmlAttr *prop = pIter;
        xml_private_t *p = pIter->_private;

        pIter = pIter->next;
        if(is_set(p->flags, xpf_created)) {
            char *name = strdup((const char *)prop->name);
            char *value = crm_element_value_copy(new, name);

            crm_trace("Created %s@%s=%s", new->name, name, value);
            /* Remove plus create wont work as it will modify the relative attribute ordering */
            if(__xml_acl_check(new, name, xpf_acl_write)) {
                crm_attr_dirty(prop);
            } else {
                xmlUnsetProp(new, prop->name); /* Remove - change not allowed */
            }

            free(value);
            free(name);
        }
    }

    for (cIter = __xml_first_child(old); cIter != NULL; ) {
        xmlNode *old_child = cIter;
        xmlNode *new_child = find_entity(new, crm_element_name(cIter), ID(cIter));

        cIter = __xml_next(cIter);
        if(new_child) {
            __xml_diff_object(old_child, new_child);

        } else {
            xml_private_t *p = old_child->_private;

            /* Create then free (which will check the acls if necessary) */
            xmlNode *candidate = add_node_copy(new, old_child);
            xmlNode *top = xmlDocGetRootElement(candidate->doc);

            __xml_node_clean(candidate);
            __xml_acl_apply(top); /* Make sure any ACLs are applied to 'candidate' */
            free_xml(candidate);

            if(NULL == find_entity(new, crm_element_name(old_child), ID(old_child))) {
                p->flags |= xpf_skip;
            }
        }
    }

    for (cIter = __xml_first_child(new); cIter != NULL; ) {
        xmlNode *new_child = cIter;
        xmlNode *old_child = find_entity(old, crm_element_name(cIter), ID(cIter));

        cIter = __xml_next(cIter);
        if(old_child == NULL) {
            xml_private_t *p = new_child->_private;
            p->flags |= xpf_skip;
            __xml_diff_object(old_child, new_child);

        } else {
            /* Check for movement, we already checked for differences */
            int p_new = __xml_offset(new_child);
            int p_old = __xml_offset(old_child);
            xml_private_t *p = new_child->_private;

            if(p_old != p_new) {
                crm_info("%s.%s moved from %d to %d - %d",
                         new_child->name, ID(new_child), p_old, p_new);
                p->flags |= xpf_moved;

                if(p_old > p_new) {
                    p = old_child->_private;
                    p->flags |= xpf_skip;

                } else {
                    p = new_child->_private;
                    p->flags |= xpf_skip;
                }
            }
        }
    }
}
