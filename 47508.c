krb5_gss_inquire_cred_by_oid(OM_uint32 *minor_status,
                             const gss_cred_id_t cred_handle,
                             const gss_OID desired_object,
                             gss_buffer_set_t *data_set)
{
    OM_uint32 major_status = GSS_S_FAILURE;
#if 0
    size_t i;
#endif

    if (minor_status == NULL)
        return GSS_S_CALL_INACCESSIBLE_WRITE;

    *minor_status = 0;

    if (desired_object == GSS_C_NO_OID)
        return GSS_S_CALL_INACCESSIBLE_READ;

    if (data_set == NULL)
        return GSS_S_CALL_INACCESSIBLE_WRITE;

    *data_set = GSS_C_NO_BUFFER_SET;
    if (cred_handle == GSS_C_NO_CREDENTIAL) {
        *minor_status = (OM_uint32)KRB5_NOCREDS_SUPPLIED;
        return GSS_S_NO_CRED;
    }

    major_status = krb5_gss_validate_cred(minor_status, cred_handle);
    if (GSS_ERROR(major_status))
        return major_status;

#if 0
    for (i = 0; i < sizeof(krb5_gss_inquire_cred_by_oid_ops)/
             sizeof(krb5_gss_inquire_cred_by_oid_ops[0]); i++) {
        if (g_OID_prefix_equal(desired_object, &krb5_gss_inquire_cred_by_oid_ops[i].oid)) {
            return (*krb5_gss_inquire_cred_by_oid_ops[i].func)(minor_status,
                                                               cred_handle,
                                                               desired_object,
                                                               data_set);
        }
    }
#endif

    *minor_status = EINVAL;

    return GSS_S_UNAVAILABLE;
}
