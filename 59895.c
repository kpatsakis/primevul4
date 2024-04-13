rend_service_get_by_service_id(const char *id)
{
  tor_assert(strlen(id) == REND_SERVICE_ID_LEN_BASE32);
  SMARTLIST_FOREACH(rend_service_list, rend_service_t*, s, {
    if (tor_memeq(s->service_id, id, REND_SERVICE_ID_LEN_BASE32))
      return s;
  });
  return NULL;
}
