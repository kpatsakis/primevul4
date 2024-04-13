free_key_ctx_bi (struct key_ctx_bi *ctx)
{
  free_key_ctx(&ctx->encrypt);
  free_key_ctx(&ctx->decrypt);
}
