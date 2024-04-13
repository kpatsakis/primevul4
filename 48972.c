krb5_gss_get_mic_iov(OM_uint32 *minor_status,
                     gss_ctx_id_t context_handle,
                     gss_qop_t qop_req,
                     gss_iov_buffer_desc *iov,
                     int iov_count)
{
    OM_uint32 major_status;

    major_status = kg_seal_iov(minor_status, context_handle, FALSE,
                               qop_req, NULL,
                               iov, iov_count, KG_TOK_MIC_MSG);

    return major_status;
}
