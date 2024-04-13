add_pa_data_element(krb5_context context,
                    krb5_pa_data *padata,
                    krb5_pa_data ***inout_padata,
                    krb5_boolean copy)
{
    int                         i;
    krb5_pa_data                **p;

    if (*inout_padata != NULL) {
        for (i = 0; (*inout_padata)[i] != NULL; i++)
            ;
    } else
        i = 0;

    p = realloc(*inout_padata, (i + 2) * sizeof(krb5_pa_data *));
    if (p == NULL)
        return ENOMEM;

    *inout_padata = p;

    p[i] = (krb5_pa_data *)malloc(sizeof(krb5_pa_data));
    if (p[i] == NULL)
        return ENOMEM;
    *(p[i]) = *padata;

    p[i + 1] = NULL;

    if (copy) {
        p[i]->contents = (krb5_octet *)malloc(padata->length);
        if (p[i]->contents == NULL) {
            free(p[i]);
            p[i] = NULL;
            return ENOMEM;
        }

        memcpy(p[i]->contents, padata->contents, padata->length);
    }

    return 0;
}
