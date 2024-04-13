rend_service_get_by_pk_digest(const char* digest)
{
  SMARTLIST_FOREACH(rend_service_list, rend_service_t*, s,
                    if (tor_memeq(s->pk_digest,digest,DIGEST_LEN))
                        return s);
  return NULL;
}
