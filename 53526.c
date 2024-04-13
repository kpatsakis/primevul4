static pfunc value(struct jv_parser* p, jv val) {
  if ((p->flags & JV_PARSE_STREAMING)) {
    if (jv_is_valid(p->next) || p->last_seen == JV_LAST_VALUE)
      return "Expected separator between values";
    if (p->stacklen > 0)
      p->last_seen = JV_LAST_VALUE;
    else
      p->last_seen = JV_LAST_NONE;
  } else {
    if (jv_is_valid(p->next)) return "Expected separator between values";
  }
  jv_free(p->next);
  p->next = val;
  return 0;
}
