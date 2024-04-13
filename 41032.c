free_key_ctx (struct key_ctx *ctx)
{
  if (ctx->cipher)
    {
      cipher_ctx_cleanup(ctx->cipher);
      free(ctx->cipher);
      ctx->cipher = NULL;
    }
  if (ctx->hmac)
    {
      hmac_ctx_cleanup(ctx->hmac);
      free(ctx->hmac);
      ctx->hmac = NULL;
    }
}
