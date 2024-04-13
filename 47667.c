static gboolean patch_legacy_mode(void)
{
    static gboolean init = TRUE;
    static gboolean legacy = FALSE;

    if(init) {
        init = FALSE;
        legacy = daemon_option_enabled("cib", "legacy");
        if(legacy) {
            crm_notice("Enabled legacy mode");
        }
    }
    return legacy;
}
