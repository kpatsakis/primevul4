static int __xml_build_schema_list(void) 
{
    int lpc, max;
    const char *base = get_schema_root();
    struct dirent **namelist = NULL;

    max = scandir(base, &namelist, schema_filter, schema_sort);
    __xml_schema_add(1, 0.0, "pacemaker-0.6", "crm.dtd", "upgrade06.xsl", 3);
    __xml_schema_add(1, 0.0, "transitional-0.6", "crm-transitional.dtd", "upgrade06.xsl", 3);
    __xml_schema_add(2, 0.0, "pacemaker-0.7", "pacemaker-1.0.rng", NULL, 0);

    if (max < 0) {
        crm_notice("scandir(%s) failed: %s (%d)", base, strerror(errno), errno);

    } else {
        for (lpc = 0; lpc < max; lpc++) {
            int next = 0;
            float version = 0.0;
            char *transform = NULL;

            sscanf(namelist[lpc]->d_name, "pacemaker-%f.rng", &version);
            if((lpc + 1) < max) {
                float next_version = 0.0;

                sscanf(namelist[lpc+1]->d_name, "pacemaker-%f.rng", &next_version);

                if(floor(version) < floor(next_version)) {
                    struct stat s;
                    char *xslt = NULL;

                    transform = crm_strdup_printf("upgrade-%.1f.xsl", version);
                    xslt = get_schema_path(NULL, transform);
                    if(stat(xslt, &s) != 0) {
                        crm_err("Transform %s not found", xslt);
                        free(xslt);
                        __xml_schema_add(2, version, NULL, NULL, NULL, -1);
                        break;
                    } else {
                        free(xslt);
                    }
                }

            } else {
                next = -1;
            }
            __xml_schema_add(2, version, NULL, NULL, transform, next);
            free(namelist[lpc]);
            free(transform);
        }
    }

    /* 1.1 was the old name for -next */
    __xml_schema_add(2, 0.0, "pacemaker-1.1", "pacemaker-next.rng", NULL, 0);
    __xml_schema_add(2, 0.0, "pacemaker-next", "pacemaker-next.rng", NULL, -1);
    __xml_schema_add(0, 0.0, "none", "N/A", NULL, -1);
    free(namelist);
    return TRUE;
}
