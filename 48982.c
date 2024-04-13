gss_krb5int_free_lucid_sec_context(
    OM_uint32 *minor_status,
    const gss_OID desired_mech,
    const gss_OID desired_object,
    gss_buffer_t value)
{
    OM_uint32           retval;
    krb5_error_code     kret = 0;
    int                 version;
    void                *kctx;

    /* Assume failure */
    retval = GSS_S_FAILURE;
    *minor_status = 0;

    kctx = value->value;
    if (!kctx) {
        kret = EINVAL;
        goto error_out;
    }

    /* Determine version and call correct free routine */
    version = ((gss_krb5_lucid_context_version_t *)kctx)->version;
    switch (version) {
    case 1:
        free_external_lucid_ctx_v1((gss_krb5_lucid_context_v1_t*) kctx);
        break;
    default:
        kret = EINVAL;
        break;
    }

    if (kret)
        goto error_out;

    /* Success! */
    *minor_status = 0;
    retval = GSS_S_COMPLETE;

    return (retval);

error_out:
    if (*minor_status == 0)
        *minor_status = (OM_uint32) kret;
    return(retval);
}
