xml_apply_patchset(xmlNode *xml, xmlNode *patchset, bool check_version) 
{
    int format = 1;
    int rc = pcmk_ok;
    xmlNode *old = NULL;
    const char *digest = crm_element_value(patchset, XML_ATTR_DIGEST);

    if(patchset == NULL) {
        return rc;
    }

    xml_log_patchset(LOG_TRACE, __FUNCTION__, patchset);

    crm_element_value_int(patchset, "format", &format);
    if(check_version) {
        rc = xml_patch_version_check(xml, patchset, format);
        if(rc != pcmk_ok) {
            return rc;
        }
    }

    if(digest) {
        /* Make it available for logging if the result doesn't have the expected digest */
        old = copy_xml(xml);
    }

    if(rc == pcmk_ok) {
        switch(format) {
            case 1:
                rc = xml_apply_patchset_v1(xml, patchset, check_version);
                break;
            case 2:
                rc = xml_apply_patchset_v2(xml, patchset, check_version);
                break;
            default:
                crm_err("Unknown patch format: %d", format);
                rc = -EINVAL;
        }
    }

    if(rc == pcmk_ok && digest) {
        static struct qb_log_callsite *digest_cs = NULL;

        char *new_digest = NULL;
        char *version = crm_element_value_copy(xml, XML_ATTR_CRM_VERSION);

        if (digest_cs == NULL) {
            digest_cs =
                qb_log_callsite_get(__func__, __FILE__, "diff-digest", LOG_TRACE, __LINE__,
                                    crm_trace_nonlog);
        }

        new_digest = calculate_xml_versioned_digest(xml, FALSE, TRUE, version);
        if (safe_str_neq(new_digest, digest)) {
            crm_info("v%d digest mis-match: expected %s, calculated %s", format, digest, new_digest);
            rc = -pcmk_err_diff_failed;

            if (digest_cs && digest_cs->targets) {
                save_xml_to_file(old,     "PatchDigest:input", NULL);
                save_xml_to_file(xml,     "PatchDigest:result", NULL);
                save_xml_to_file(patchset,"PatchDigest:diff", NULL);

            } else {
                crm_trace("%p %0.6x", digest_cs, digest_cs ? digest_cs->targets : 0);
            }

        } else {
            crm_trace("v%d digest matched: expected %s, calculated %s", format, digest, new_digest);
        }
        free(new_digest);
        free(version);
    }
    free_xml(old);
    return rc;
}
