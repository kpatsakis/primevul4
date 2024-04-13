static void parser_reset(struct jv_parser* p) {
  if ((p->flags & JV_PARSE_STREAMING)) {
    jv_free(p->path);
    p->path = jv_array();
    p->stacklen = 0;
  }
  p->last_seen = JV_LAST_NONE;
  jv_free(p->output);
  p->output = jv_invalid();
  jv_free(p->next);
  p->next = jv_invalid();
  for (int i=0; i<p->stackpos; i++)
    jv_free(p->stack[i]);
  p->stackpos = 0;
  p->tokenpos = 0;
  p->st = JV_PARSER_NORMAL;
}
