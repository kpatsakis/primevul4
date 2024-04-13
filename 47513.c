krb5_gssspi_mech_invoke (OM_uint32 *minor_status,
                         const gss_OID desired_mech,
                         const gss_OID desired_object,
                         gss_buffer_t value)
{
    size_t i;

    if (minor_status == NULL)
        return GSS_S_CALL_INACCESSIBLE_WRITE;

    *minor_status = 0;

    if (desired_mech == GSS_C_NO_OID)
        return GSS_S_BAD_MECH;

    if (desired_object == GSS_C_NO_OID)
        return GSS_S_CALL_INACCESSIBLE_READ;

    for (i = 0; i < sizeof(krb5_gssspi_mech_invoke_ops)/
             sizeof(krb5_gssspi_mech_invoke_ops[0]); i++) {
        if (g_OID_prefix_equal(desired_object, &krb5_gssspi_mech_invoke_ops[i].oid)) {
            return (*krb5_gssspi_mech_invoke_ops[i].func)(minor_status,
                                                          desired_mech,
                                                          desired_object,
                                                          value);
        }
    }

    *minor_status = EINVAL;

    return GSS_S_UNAVAILABLE;
}
