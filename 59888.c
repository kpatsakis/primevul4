rend_service_derive_key_digests(struct rend_service_t *s)
{
  if (rend_get_service_id(s->private_key, s->service_id)<0) {
    log_warn(LD_BUG, "Internal error: couldn't encode service ID.");
    return -1;
  }
  if (crypto_pk_get_digest(s->private_key, s->pk_digest)<0) {
    log_warn(LD_BUG, "Couldn't compute hash of public key.");
    return -1;
  }

  return 0;
}
