krb5_is_tgs_principal(krb5_const_principal principal)
{
    if (krb5_princ_size(kdc_context, principal) != 2)
        return FALSE;
    if (data_eq_string(*krb5_princ_component(kdc_context, principal, 0),
                       KRB5_TGS_NAME))
        return TRUE;
    else
        return FALSE;
}
