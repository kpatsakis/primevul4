static int schema_sort(const struct dirent ** a, const struct dirent **b)
{
    int rc = 0;
    float a_version = 0.0;
    float b_version = 0.0;

    sscanf(a[0]->d_name, "pacemaker-%f.rng", &a_version);
    sscanf(b[0]->d_name, "pacemaker-%f.rng", &b_version);

    if(a_version > b_version) {
        rc = 1;
    } else if(a_version < b_version) {
        rc = -1;
    }

    /* crm_trace("%s (%f) vs. %s (%f) : %d", a[0]->d_name, a_version, b[0]->d_name, b_version, rc); */
    return rc;
}
