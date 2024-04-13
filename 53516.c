static void parser_free(struct jv_parser* p) {
  parser_reset(p);
  jv_free(p->path);
  jv_free(p->output);
  jv_mem_free(p->stack);
  jv_mem_free(p->tokenbuf);
  jvp_dtoa_context_free(&p->dtoa);
}
