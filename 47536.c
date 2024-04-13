iakerb_is_iakerb_token(const gss_buffer_t token)
{
    krb5_error_code code;
    unsigned int bodysize = token->length;
    unsigned char *ptr = token->value;

    code = g_verify_token_header(gss_mech_iakerb,
                                 &bodysize, &ptr,
                                 IAKERB_TOK_PROXY,
                                 token->length, 0);

    return (code == 0);
}
