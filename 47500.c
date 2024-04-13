static int gss_krb5mechglue_init(void)
{
    struct gss_mech_config mech_krb5;

    memset(&mech_krb5, 0, sizeof(mech_krb5));
    mech_krb5.mech = &krb5_mechanism;

    mech_krb5.mechNameStr = "kerberos_v5";
    mech_krb5.mech_type = (gss_OID)gss_mech_krb5;
    gssint_register_mechinfo(&mech_krb5);

    mech_krb5.mechNameStr = "kerberos_v5_old";
    mech_krb5.mech_type = (gss_OID)gss_mech_krb5_old;
    gssint_register_mechinfo(&mech_krb5);

    mech_krb5.mechNameStr = "mskrb";
    mech_krb5.mech_type = (gss_OID)gss_mech_krb5_wrong;
    gssint_register_mechinfo(&mech_krb5);

    return 0;
}
