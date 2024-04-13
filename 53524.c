static pfunc stream_token(struct jv_parser* p, char ch) {
  jv_kind k;
  jv last;

  switch (ch) {
  case '[':
    if (jv_is_valid(p->next))
      return "Expected a separator between values";
    p->path = jv_array_append(p->path, jv_number(0)); // push
    p->last_seen = JV_LAST_OPEN_ARRAY;
    p->stacklen++;
    break;

  case '{':
    if (p->last_seen == JV_LAST_VALUE)
      return "Expected a separator between values";
    p->path = jv_array_append(p->path, jv_null()); // push
    p->last_seen = JV_LAST_OPEN_OBJECT;
    p->stacklen++;
    break;

  case ':':
    if (p->stacklen == 0 || jv_get_kind(jv_array_get(jv_copy(p->path), p->stacklen - 1)) == JV_KIND_NUMBER)
      return "':' not as part of an object";
    if (!jv_is_valid(p->next) || p->last_seen == JV_LAST_NONE)
      return "Expected string key before ':'";
    if (jv_get_kind(p->next) != JV_KIND_STRING)
      return "Object keys must be strings";
    if (p->last_seen != JV_LAST_VALUE)
      return "':' should follow a key";
    p->last_seen = JV_LAST_COLON;
    p->path = jv_array_set(p->path, p->stacklen - 1, p->next);
    p->next = jv_invalid();
    break;

  case ',':
    if (p->last_seen != JV_LAST_VALUE)
      return "Expected value before ','";
    if (p->stacklen == 0)
      return "',' not as part of an object or array";
    last = jv_array_get(jv_copy(p->path), p->stacklen - 1);
    k = jv_get_kind(last);
    if (k == JV_KIND_NUMBER) {
      int idx = jv_number_value(last);

      if (jv_is_valid(p->next)) {
        p->output = JV_ARRAY(jv_copy(p->path), p->next);
        p->next = jv_invalid();
      }
      p->path = jv_array_set(p->path, p->stacklen - 1, jv_number(idx + 1));
      p->last_seen = JV_LAST_COMMA;
    } else if (k == JV_KIND_STRING) {
      if (jv_is_valid(p->next)) {
        p->output = JV_ARRAY(jv_copy(p->path), p->next);
        p->next = jv_invalid();
      }
      p->path = jv_array_set(p->path, p->stacklen - 1, jv_true()); // ready for another name:value pair
      p->last_seen = JV_LAST_COMMA;
    } else {
      assert(k == JV_KIND_NULL);
      jv_free(last);
      return "Objects must consist of key:value pairs";
    }
    jv_free(last);
    break;

  case ']':
    if (p->stacklen == 0)
      return "Unmatched ']' at the top-level";
    if (p->last_seen == JV_LAST_COMMA)
      return "Expected another array element";
    if (p->last_seen == JV_LAST_OPEN_ARRAY)
      assert(!jv_is_valid(p->next));

    last = jv_array_get(jv_copy(p->path), p->stacklen - 1);
    k = jv_get_kind(last);
    jv_free(last);

    if (k != JV_KIND_NUMBER)
      return "Unmatched ']' in the middle of an object";
    if (jv_is_valid(p->next)) {
      p->output = JV_ARRAY(jv_copy(p->path), p->next, jv_true());
      p->next = jv_invalid();
    } else if (p->last_seen != JV_LAST_OPEN_ARRAY) {
      p->output = JV_ARRAY(jv_copy(p->path));
    }

    p->path = jv_array_slice(p->path, 0, --(p->stacklen)); // pop
    jv_free(p->next);
    p->next = jv_invalid();

    if (p->last_seen == JV_LAST_OPEN_ARRAY)
      p->output = JV_ARRAY(jv_copy(p->path), jv_array()); // Empty arrays are leaves

    if (p->stacklen == 0)
      p->last_seen = JV_LAST_NONE;
    else
      p->last_seen = JV_LAST_VALUE;
    break;

  case '}':
    if (p->stacklen == 0)
      return "Unmatched '}' at the top-level";
    if (p->last_seen == JV_LAST_COMMA)
      return "Expected another key:value pair";
    if (p->last_seen == JV_LAST_OPEN_OBJECT)
      assert(!jv_is_valid(p->next));

    last = jv_array_get(jv_copy(p->path), p->stacklen - 1);
    k = jv_get_kind(last);
    jv_free(last);
    if (k == JV_KIND_NUMBER)
      return "Unmatched '}' in the middle of an array";
