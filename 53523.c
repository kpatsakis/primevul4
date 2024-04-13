static int stream_is_top_num(struct jv_parser* p) {
  return (p->stacklen == 0 && jv_get_kind(p->next) == JV_KIND_NUMBER);
}
