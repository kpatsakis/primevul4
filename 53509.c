int jv_parser_remaining(struct jv_parser* p) {
  if (p->curr_buf == 0)
    return 0;
  return (p->curr_buf_length - p->curr_buf_pos);
}
