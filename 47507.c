krb5_gss_inquire_attrs_for_mech(OM_uint32 *minor_status,
                                gss_const_OID mech,
                                gss_OID_set *mech_attrs,
                                gss_OID_set *known_mech_attrs)
{
    OM_uint32 major, tmpMinor;

    if (mech_attrs == NULL) {
        *minor_status = 0;
        return GSS_S_COMPLETE;
    }

    major = gss_create_empty_oid_set(minor_status, mech_attrs);
    if (GSS_ERROR(major))
        goto cleanup;

#define MA_SUPPORTED(ma)    do {                                        \
        major = gss_add_oid_set_member(minor_status, (gss_OID)ma,       \
                                       mech_attrs);                     \
        if (GSS_ERROR(major))                                           \
            goto cleanup;                                               \
    } while (0)

    MA_SUPPORTED(GSS_C_MA_MECH_CONCRETE);
    MA_SUPPORTED(GSS_C_MA_ITOK_FRAMED);
    MA_SUPPORTED(GSS_C_MA_AUTH_INIT);
    MA_SUPPORTED(GSS_C_MA_AUTH_TARG);
    MA_SUPPORTED(GSS_C_MA_DELEG_CRED);
    MA_SUPPORTED(GSS_C_MA_INTEG_PROT);
    MA_SUPPORTED(GSS_C_MA_CONF_PROT);
    MA_SUPPORTED(GSS_C_MA_MIC);
    MA_SUPPORTED(GSS_C_MA_WRAP);
    MA_SUPPORTED(GSS_C_MA_PROT_READY);
    MA_SUPPORTED(GSS_C_MA_REPLAY_DET);
    MA_SUPPORTED(GSS_C_MA_OOS_DET);
    MA_SUPPORTED(GSS_C_MA_CBINDINGS);
    MA_SUPPORTED(GSS_C_MA_CTX_TRANS);

    if (g_OID_equal(mech, gss_mech_iakerb)) {
        MA_SUPPORTED(GSS_C_MA_AUTH_INIT_INIT);
        MA_SUPPORTED(GSS_C_MA_NOT_DFLT_MECH);
    } else if (!g_OID_equal(mech, gss_mech_krb5)) {
        MA_SUPPORTED(GSS_C_MA_DEPRECATED);
    }

cleanup:
    if (GSS_ERROR(major))
        gss_release_oid_set(&tmpMinor, mech_attrs);

    return major;
}
