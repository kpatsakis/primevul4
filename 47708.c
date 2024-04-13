xml_patch_version_check(xmlNode *xml, xmlNode *patchset, int format) 
{
    int lpc = 0;
    bool changed = FALSE;

    int this[] = { 0, 0, 0 };
    int add[] = { 0, 0, 0 };
    int del[] = { 0, 0, 0 };

    const char *vfields[] = {
        XML_ATTR_GENERATION_ADMIN,
        XML_ATTR_GENERATION,
        XML_ATTR_NUMUPDATES,
    };

    for(lpc = 0; lpc < DIMOF(vfields); lpc++) {
        crm_element_value_int(xml, vfields[lpc], &(this[lpc]));
        crm_trace("Got %d for this[%s]", this[lpc], vfields[lpc]);
        if (this[lpc] < 0) {
            this[lpc] = 0;
        }
    }

    /* Set some defaults in case nothing is present */
    add[0] = this[0];
    add[1] = this[1];
    add[2] = this[2] + 1;
    for(lpc = 0; lpc < DIMOF(vfields); lpc++) {
        del[lpc] = this[lpc];
    }

    xml_patch_versions(patchset, add, del);

    for(lpc = 0; lpc < DIMOF(vfields); lpc++) {
        if(this[lpc] < del[lpc]) {
            crm_debug("Current %s is too low (%d < %d)", vfields[lpc], this[lpc], del[lpc]);
            return -pcmk_err_diff_resync;

        } else if(this[lpc] > del[lpc]) {
            crm_info("Current %s is too high (%d > %d)", vfields[lpc], this[lpc], del[lpc]);
            return -pcmk_err_old_data;
        }
    }

    for(lpc = 0; lpc < DIMOF(vfields); lpc++) {
        if(add[lpc] > del[lpc]) {
            changed = TRUE;
        }
    }

    if(changed == FALSE) {
        crm_notice("Versions did not change in patch %d.%d.%d", add[0], add[1], add[2]);
        return -pcmk_err_old_data;
    }

    crm_debug("Can apply patch %d.%d.%d to %d.%d.%d",
             add[0], add[1], add[2], this[0], this[1], this[2]);
    return pcmk_ok;
}
