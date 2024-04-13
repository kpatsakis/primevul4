rend_service_escaped_dir(const struct rend_service_t *s)
{
  return rend_service_is_ephemeral(s) ? "[EPHEMERAL]" : escaped(s->directory);
}
