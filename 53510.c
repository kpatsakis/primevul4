void jv_parser_set_buf(struct jv_parser* p, const char* buf, int length, int is_partial) {
  assert((p->curr_buf == 0 || p->curr_buf_pos == p->curr_buf_length)
         && "previous buffer not exhausted");
  while (length > 0 && p->bom_strip_position < sizeof(UTF8_BOM)) {
    if ((unsigned char)*buf == UTF8_BOM[p->bom_strip_position]) {
      buf++;
      length--;
      p->bom_strip_position++;
    } else {
      if (p->bom_strip_position == 0) {
        p->bom_strip_position = sizeof(UTF8_BOM);
      } else {
        p->bom_strip_position = 0xff;
      }
    }
  }
  p->curr_buf = buf;
  p->curr_buf_length = length;
  p->curr_buf_pos = 0;
  p->curr_buf_is_partial = is_partial;
}
