static pfunc scan(struct jv_parser* p, char ch, jv* out) {
  p->column++;
  if (ch == '\n') {
    p->line++;
    p->column = 0;
  }
  if (ch == '\036' /* ASCII RS; see draft-ietf-json-sequence-07 */) {
    if (check_truncation(p)) {
      if (check_literal(p) == 0 && is_top_num(p))
        return "Potentially truncated top-level numeric value";
      return "Truncated value";
    }
    TRY(check_literal(p));
    if (p->st == JV_PARSER_NORMAL && check_done(p, out))
      return OK;
    assert(!jv_is_valid(*out));
    parser_reset(p);
    jv_free(*out);
    *out = jv_invalid();
    return OK;
  }
  presult answer = 0;
  p->last_ch_was_ws = 0;
  if (p->st == JV_PARSER_NORMAL) {
    chclass cls = classify(ch);
    if (cls == WHITESPACE)
      p->last_ch_was_ws = 1;
    if (cls != LITERAL) {
      TRY(check_literal(p));
      if (check_done(p, out)) answer = OK;
    }
    switch (cls) {
    case LITERAL:
      tokenadd(p, ch);
      break;
    case WHITESPACE:
      break;
    case QUOTE:
      p->st = JV_PARSER_STRING;
      break;
    case STRUCTURE:
      TRY(token(p, ch));
      break;
    case INVALID:
      return "Invalid character";
    }
    if (check_done(p, out)) answer = OK;
  } else {
    if (ch == '"' && p->st == JV_PARSER_STRING) {
      TRY(found_string(p));
      p->st = JV_PARSER_NORMAL;
      if (check_done(p, out)) answer = OK;
    } else {
      tokenadd(p, ch);
      if (ch == '\\' && p->st == JV_PARSER_STRING) {
        p->st = JV_PARSER_STRING_ESCAPE;
      } else {
        p->st = JV_PARSER_STRING;
      }
    }
  }
  return answer;
}
