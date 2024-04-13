krb5_gss_get_mic(minor_status, context_handle, qop_req,
                 message_buffer, message_token)
    OM_uint32           *minor_status;
    gss_ctx_id_t        context_handle;
    gss_qop_t           qop_req;
    gss_buffer_t        message_buffer;
    gss_buffer_t        message_token;
{
    return(kg_seal(minor_status, context_handle, 0,
                   qop_req, message_buffer, NULL,
                   message_token, KG_TOK_MIC_MSG));
}
