rend_service_load_all_keys(const smartlist_t *service_list)
{
  /* Use service_list for unit tests */
  const smartlist_t *s_list = rend_get_service_list(service_list);
  if (BUG(!s_list)) {
    return -1;
  }

  SMARTLIST_FOREACH_BEGIN(s_list, rend_service_t *, s) {
    if (s->private_key)
      continue;
    log_info(LD_REND, "Loading hidden-service keys from %s",
             rend_service_escaped_dir(s));

    if (rend_service_load_keys(s) < 0)
      return -1;
  } SMARTLIST_FOREACH_END(s);

  return 0;
}
