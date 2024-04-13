krb5_gss_authorize_localname(OM_uint32 *minor,
                             const gss_name_t pname,
                             gss_const_buffer_t local_user,
                             gss_const_OID name_type)
{
    krb5_context context;
    krb5_error_code code;
    krb5_gss_name_t kname;
    char *user;
    int user_ok;

    if (name_type != GSS_C_NO_OID &&
        !g_OID_equal(name_type, GSS_C_NT_USER_NAME)) {
        return GSS_S_BAD_NAMETYPE;
    }

    kname = (krb5_gss_name_t)pname;

    code = krb5_gss_init_context(&context);
    if (code != 0) {
        *minor = code;
        return GSS_S_FAILURE;
    }

    user = k5memdup0(local_user->value, local_user->length, &code);
    if (user == NULL) {
        *minor = code;
        krb5_free_context(context);
        return GSS_S_FAILURE;
    }

    user_ok = krb5_kuserok(context, kname->princ, user);

    free(user);
    krb5_free_context(context);

    *minor = 0;
    return user_ok ? GSS_S_COMPLETE : GSS_S_UNAUTHORIZED;
}
