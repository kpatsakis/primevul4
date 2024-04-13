static int schema_filter(const struct dirent * a)
{
    int rc = 0;
    float version = 0;

    if(strstr(a->d_name, "pacemaker-") != a->d_name) {
        /* crm_trace("%s - wrong prefix", a->d_name); */

    } else if(strstr(a->d_name, ".rng") == NULL) {
        /* crm_trace("%s - wrong suffix", a->d_name); */

    } else if(sscanf(a->d_name, "pacemaker-%f.rng", &version) == 0) {
        /* crm_trace("%s - wrong format", a->d_name); */

    } else if(strcmp(a->d_name, "pacemaker-1.1.rng") == 0) {
        /* crm_trace("%s - hack", a->d_name); */

    } else {
        /* crm_debug("%s - candidate", a->d_name); */
        rc = 1;
    }

    return rc;
}
