kg_get_defcred(minor_status, cred)
    OM_uint32 *minor_status;
    gss_cred_id_t *cred;
{
    OM_uint32 major;

    if ((major = krb5_gss_acquire_cred(minor_status,
                                       (gss_name_t) NULL, GSS_C_INDEFINITE,
                                       GSS_C_NULL_OID_SET, GSS_C_INITIATE,
                                       cred, NULL, NULL)) && GSS_ERROR(major)) {
        return(major);
    }
    *minor_status = 0;
    return(GSS_S_COMPLETE);
}
