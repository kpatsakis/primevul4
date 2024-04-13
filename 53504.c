jv jv_parse(const char* string) {
  return jv_parse_sized(string, strlen(string));
}
