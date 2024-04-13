concat_authorization_data(krb5_context context,
                          krb5_authdata **first, krb5_authdata **second,
                          krb5_authdata ***output)
{
    register int i, j;
    register krb5_authdata **ptr, **retdata;

    /* count up the entries */
    i = 0;
    if (first)
        for (ptr = first; *ptr; ptr++)
            i++;
    if (second)
        for (ptr = second; *ptr; ptr++)
            i++;

    retdata = (krb5_authdata **)malloc((i+1)*sizeof(*retdata));
    if (!retdata)
        return ENOMEM;
    retdata[i] = 0;                     /* null-terminated array */
    for (i = 0, j = 0, ptr = first; j < 2 ; ptr = second, j++)
        while (ptr && *ptr) {
            /* now walk & copy */
            retdata[i] = (krb5_authdata *)malloc(sizeof(*retdata[i]));
            if (!retdata[i]) {
                krb5_free_authdata(context, retdata);
                return ENOMEM;
            }
            *retdata[i] = **ptr;
            if (!(retdata[i]->contents =
                  (krb5_octet *)malloc(retdata[i]->length))) {
                free(retdata[i]);
                retdata[i] = 0;
                krb5_free_authdata(context, retdata);
                return ENOMEM;
            }
            memcpy(retdata[i]->contents, (*ptr)->contents, retdata[i]->length);

            ptr++;
            i++;
        }
    *output = retdata;
    return 0;
}
