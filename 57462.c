int yr_re_parse(
    const char* re_string,
    RE_AST** re_ast,
    RE_ERROR* error)
{
  return yr_parse_re_string(re_string, re_ast, error);
}
