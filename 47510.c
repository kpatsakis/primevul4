krb5_gss_inquire_saslname_for_mech(OM_uint32 *minor_status,
                                   const gss_OID desired_mech,
                                   gss_buffer_t sasl_mech_name,
                                   gss_buffer_t mech_name,
                                   gss_buffer_t mech_description)
{
    if (g_OID_equal(desired_mech, gss_mech_iakerb)) {
        if (!g_make_string_buffer(GS2_IAKERB_SASL_NAME, sasl_mech_name) ||
            !g_make_string_buffer("iakerb", mech_name) ||
            !g_make_string_buffer("Initial and Pass Through Authentication "
                                  "Kerberos Mechanism (IAKERB)",
                                  mech_description))
            goto fail;
    } else {
        if (!g_make_string_buffer(GS2_KRB5_SASL_NAME, sasl_mech_name) ||
            !g_make_string_buffer("krb5", mech_name) ||
            !g_make_string_buffer("Kerberos 5 GSS-API Mechanism",
                                  mech_description))
            goto fail;
    }

    *minor_status = 0;
    return GSS_S_COMPLETE;

fail:
    *minor_status = ENOMEM;
    return GSS_S_FAILURE;
}
