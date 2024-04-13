is_cross_tgs_principal(krb5_const_principal principal)
{
    if (!krb5_is_tgs_principal(principal))
        return FALSE;
    if (!data_eq(*krb5_princ_component(kdc_context, principal, 1),
                 *krb5_princ_realm(kdc_context, principal)))
        return TRUE;
    else
        return FALSE;
}
