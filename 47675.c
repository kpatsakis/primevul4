sort_pairs(gconstpointer a, gconstpointer b)
{
    int rc = 0;
    const name_value_t *pair_a = a;
    const name_value_t *pair_b = b;

    CRM_ASSERT(a != NULL);
    CRM_ASSERT(pair_a->name != NULL);

    CRM_ASSERT(b != NULL);
    CRM_ASSERT(pair_b->name != NULL);

    rc = strcmp(pair_a->name, pair_b->name);
    if (rc < 0) {
        return -1;
    } else if (rc > 0) {
        return 1;
    }
    return 0;
}
