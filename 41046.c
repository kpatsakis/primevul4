md5_state_final (struct md5_state *s, struct md5_digest *out)
{
  md_ctx_final(&s->ctx, out->digest);
  md_ctx_cleanup(&s->ctx);
}
