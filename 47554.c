otp_init(krb5_context context, krb5_kdcpreauth_moddata *moddata_out,
         const char **realmnames)
{
    krb5_error_code retval;
    otp_state *state;

    retval = otp_state_new(context, &state);
    if (retval)
        return retval;
    *moddata_out = (krb5_kdcpreauth_moddata)state;
    return 0;
}
