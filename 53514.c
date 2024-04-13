static int parse_is_top_num(struct jv_parser* p) {
  return (p->stackpos == 0 && jv_get_kind(p->next) == JV_KIND_NUMBER);
}
