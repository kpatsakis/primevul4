gss_krb5int_extract_authz_data_from_sec_context(
    OM_uint32 *minor_status,
    const gss_ctx_id_t context_handle,
    const gss_OID desired_object,
    gss_buffer_set_t *data_set)
{
    OM_uint32 major_status;
    krb5_gss_ctx_id_rec *ctx;
    int ad_type = 0;
    size_t i;

    *data_set = GSS_C_NO_BUFFER_SET;

    ctx = (krb5_gss_ctx_id_rec *) context_handle;

    major_status = generic_gss_oid_decompose(minor_status,
                                             GSS_KRB5_EXTRACT_AUTHZ_DATA_FROM_SEC_CONTEXT_OID,
                                             GSS_KRB5_EXTRACT_AUTHZ_DATA_FROM_SEC_CONTEXT_OID_LENGTH,
                                             desired_object,
                                             &ad_type);
    if (major_status != GSS_S_COMPLETE || ad_type == 0) {
        *minor_status = ENOENT;
        return GSS_S_FAILURE;
    }

    if (ctx->authdata != NULL) {
        for (i = 0; ctx->authdata[i] != NULL; i++) {
            if (ctx->authdata[i]->ad_type == ad_type) {
                gss_buffer_desc ad_data;

                ad_data.length = ctx->authdata[i]->length;
                ad_data.value = ctx->authdata[i]->contents;

                major_status = generic_gss_add_buffer_set_member(minor_status,
                                                                 &ad_data, data_set);
                if (GSS_ERROR(major_status))
                    break;
            }
        }
    }

    if (GSS_ERROR(major_status)) {
        OM_uint32 tmp;

        generic_gss_release_buffer_set(&tmp, data_set);
    }

    return major_status;
}
