static int parse_check_done(struct jv_parser* p, jv* out) {
  if (p->stackpos == 0 && jv_is_valid(p->next)) {
    *out = p->next;
    p->next = jv_invalid();
    return 1;
  } else {
    return 0;
  }
}
