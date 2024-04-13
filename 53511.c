static jv make_error(struct jv_parser* p, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  jv e = jv_string_vfmt(fmt, ap);
  va_end(ap);
  if ((p->flags & JV_PARSE_STREAM_ERRORS))
    return JV_ARRAY(e, jv_copy(p->path));
  return jv_invalid_with_msg(e);
}
