jv jv_parse_sized(const char* string, int length) {
  struct jv_parser parser;
  parser_init(&parser, 0);
  jv_parser_set_buf(&parser, string, length, 0);
  jv value = jv_parser_next(&parser);
  if (jv_is_valid(value)) {
    jv next = jv_parser_next(&parser);
    if (jv_is_valid(next)) {
      jv_free(value);
      jv_free(next);
      value = jv_invalid_with_msg(jv_string("Unexpected extra JSON values"));
    } else if (jv_invalid_has_msg(jv_copy(next))) {
      jv_free(value);
      value = next;
    } else {
      jv_free(next);
    }
  } else if (jv_invalid_has_msg(jv_copy(value))) {
  } else {
    jv_free(value);
    value = jv_invalid_with_msg(jv_string("Expected JSON value"));
  }
  parser_free(&parser);

  if (!jv_is_valid(value) && jv_invalid_has_msg(jv_copy(value))) {
    jv msg = jv_invalid_get_msg(value);
    value = jv_invalid_with_msg(jv_string_fmt("%s (while parsing '%s')",
                                              jv_string_value(msg),
                                              string));
    jv_free(msg);
  }
  return value;
}
