static int stream_check_truncation(struct jv_parser* p) {
  jv_kind k = jv_get_kind(p->next);
  return (p->stacklen > 0 || k == JV_KIND_NUMBER || k == JV_KIND_TRUE || k == JV_KIND_FALSE || k == JV_KIND_NULL);
}
