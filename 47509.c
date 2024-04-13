krb5_gss_inquire_mech_for_saslname(OM_uint32 *minor_status,
                                   const gss_buffer_t sasl_mech_name,
                                   gss_OID *mech_type)
{
    *minor_status = 0;

    if (sasl_mech_name->length == GS2_KRB5_SASL_NAME_LEN &&
        memcmp(sasl_mech_name->value,
               GS2_KRB5_SASL_NAME, GS2_KRB5_SASL_NAME_LEN) == 0) {
        if (mech_type != NULL)
            *mech_type = (gss_OID)gss_mech_krb5;
        return GSS_S_COMPLETE;
    } else if (sasl_mech_name->length == GS2_IAKERB_SASL_NAME_LEN &&
               memcmp(sasl_mech_name->value,
                      GS2_IAKERB_SASL_NAME, GS2_IAKERB_SASL_NAME_LEN) == 0) {
        if (mech_type != NULL)
            *mech_type = (gss_OID)gss_mech_iakerb;
        return GSS_S_COMPLETE;
    }

    return GSS_S_BAD_MECH;
}
