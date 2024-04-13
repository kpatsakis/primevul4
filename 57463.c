int yr_re_parse_hex(
    const char* hex_string,
    RE_AST** re_ast,
    RE_ERROR* error)
{
  return yr_parse_hex_string(hex_string, re_ast, error);
}
