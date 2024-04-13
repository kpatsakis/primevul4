jv jv_parser_next(struct jv_parser* p) {
  if (p->eof)
    return jv_invalid();
  if (!p->curr_buf)
    return jv_invalid(); // Need a buffer
  if (p->bom_strip_position == 0xff) {
    if (!(p->flags & JV_PARSE_SEQ))
      return jv_invalid_with_msg(jv_string("Malformed BOM"));
    p->st =JV_PARSER_WAITING_FOR_RS;
    parser_reset(p);
  }
  jv value = jv_invalid();
  if ((p->flags & JV_PARSE_STREAMING) && stream_check_done(p, &value))
    return value;
  char ch;
  presult msg = 0;
  while (!msg && p->curr_buf_pos < p->curr_buf_length) {
    ch = p->curr_buf[p->curr_buf_pos++];
    if (p->st == JV_PARSER_WAITING_FOR_RS) {
      if (ch == '\n') {
        p->line++;
        p->column = 0;
      } else {
        p->column++;
      }
      if (ch == '\036')
        p->st = JV_PARSER_NORMAL;
      continue; // need to resync, wait for RS
    }
    msg = scan(p, ch, &value);
  }
  if (msg == OK) {
    return value;
  } else if (msg) {
    jv_free(value);
    if (ch != '\036' && (p->flags & JV_PARSE_SEQ)) {
      p->st = JV_PARSER_WAITING_FOR_RS;
      value = make_error(p, "%s at line %d, column %d (need RS to resync)", msg, p->line, p->column);
      parser_reset(p);
      return value;
    }
    value = make_error(p, "%s at line %d, column %d", msg, p->line, p->column);
    parser_reset(p);
    if (!(p->flags & JV_PARSE_SEQ)) {
      p->curr_buf = 0;
      p->curr_buf_pos = 0;
    } // Else ch must be RS; don't clear buf so we can start parsing again after this ch
    return value;
  } else if (p->curr_buf_is_partial) {
    assert(p->curr_buf_pos == p->curr_buf_length);
    return jv_invalid();
  } else {
    p->eof = 1;
    assert(p->curr_buf_pos == p->curr_buf_length);
    jv_free(value);
    if (p->st == JV_PARSER_WAITING_FOR_RS)
      return make_error(p, "Unfinished abandoned text at EOF at line %d, column %d", p->line, p->column);
    if (p->st != JV_PARSER_NORMAL) {
      value = make_error(p, "Unfinished string at EOF at line %d, column %d", p->line, p->column);
      parser_reset(p);
      p->st = JV_PARSER_WAITING_FOR_RS;
      return value;
    }
    if ((msg = check_literal(p))) {
      value = make_error(p, "%s at EOF at line %d, column %d", msg, p->line, p->column);
      parser_reset(p);
      p->st = JV_PARSER_WAITING_FOR_RS;
      return value;
    }
    if (((p->flags & JV_PARSE_STREAMING) && p->stacklen != 0) ||
        (!(p->flags & JV_PARSE_STREAMING) && p->stackpos != 0)) {
      value = make_error(p, "Unfinished JSON term at EOF at line %d, column %d", p->line, p->column);
      parser_reset(p);
      p->st = JV_PARSER_WAITING_FOR_RS;
      return value;
    }
    if ((p->flags & JV_PARSE_STREAMING) && jv_is_valid(p->next)) {
      value = JV_ARRAY(jv_copy(p->path), p->next); // except in streaming mode we've got to make it [path,value]
    } else {
      value = p->next;
    }
    p->next = jv_invalid();
    if ((p->flags & JV_PARSE_SEQ) && !p->last_ch_was_ws && jv_get_kind(value) == JV_KIND_NUMBER) {
      jv_free(value);
      return make_error(p, "Potentially truncated top-level numeric value at EOF at line %d, column %d", p->line, p->column);
    }
    return value;
  }
}
