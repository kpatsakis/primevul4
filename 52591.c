get_local_tgt(krb5_context context, const krb5_data *realm,
              krb5_db_entry *candidate, krb5_db_entry **alias_out,
              krb5_db_entry **storage_out)
{
    krb5_error_code ret;
    krb5_principal princ;
    krb5_db_entry *tgt;

    *alias_out = NULL;
    *storage_out = NULL;

    ret = krb5_build_principal_ext(context, &princ, realm->length, realm->data,
                                   KRB5_TGS_NAME_SIZE, KRB5_TGS_NAME,
                                   realm->length, realm->data, 0);
    if (ret)
        return ret;

    if (!krb5_principal_compare(context, candidate->princ, princ)) {
        ret = krb5_db_get_principal(context, princ, 0, &tgt);
        if (!ret)
            *storage_out = *alias_out = tgt;
    } else {
        *alias_out = candidate;
    }

    krb5_free_principal(context, princ);
    return ret;
}
