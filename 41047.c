md5_state_init (struct md5_state *s)
{
  const md_kt_t *md5_kt = md_kt_get("MD5");

  md_ctx_init(&s->ctx, md5_kt);
}
