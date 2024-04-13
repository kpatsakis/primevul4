iakerb_save_token(iakerb_ctx_id_t ctx, const gss_buffer_t token)
{
    char *p;

    p = realloc(ctx->conv.data, ctx->conv.length + token->length);
    if (p == NULL)
        return ENOMEM;

    memcpy(p + ctx->conv.length, token->value, token->length);
    ctx->conv.data = p;
    ctx->conv.length += token->length;

    return 0;
}
