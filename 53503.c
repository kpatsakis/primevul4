static pfunc found_string(struct jv_parser* p) {
  char* in = p->tokenbuf;
  char* out = p->tokenbuf;
  char* end = p->tokenbuf + p->tokenpos;

  while (in < end) {
    char c = *in++;
    if (c == '\\') {
      if (in >= end)
        return "Expected escape character at end of string";
      c = *in++;
      switch (c) {
      case '\\':
      case '"':
      case '/': *out++ = c;    break;
      case 'b': *out++ = '\b'; break;
      case 'f': *out++ = '\f'; break;
      case 't': *out++ = '\t'; break;
      case 'n': *out++ = '\n'; break;
      case 'r': *out++ = '\r'; break;

      case 'u':
        /* ahh, the complicated case */
        if (in + 4 > end)
          return "Invalid \\uXXXX escape";
        int hexvalue = unhex4(in);
        if (hexvalue < 0)
          return "Invalid characters in \\uXXXX escape";
        unsigned long codepoint = (unsigned long)hexvalue;
        in += 4;
        if (0xD800 <= codepoint && codepoint <= 0xDBFF) {
          /* who thought UTF-16 surrogate pairs were a good idea? */
          if (in + 6 > end || in[0] != '\\' || in[1] != 'u')
            return "Invalid \\uXXXX\\uXXXX surrogate pair escape";
          unsigned long surrogate = unhex4(in+2);
          if (!(0xDC00 <= surrogate && surrogate <= 0xDFFF))
            return "Invalid \\uXXXX\\uXXXX surrogate pair escape";
          in += 6;
          codepoint = 0x10000 + (((codepoint - 0xD800) << 10)
                                 |(surrogate - 0xDC00));
        }
        if (codepoint > 0x10FFFF)
          codepoint = 0xFFFD; // U+FFFD REPLACEMENT CHARACTER
        out += jvp_utf8_encode(codepoint, out);
        break;

      default:
        return "Invalid escape";
      }
    } else {
      if (c > 0 && c < 0x001f)
        return "Invalid string: control characters from U+0000 through U+001F must be escaped";
      *out++ = c;
    }
  }
  TRY(value(p, jv_string_sized(p->tokenbuf, out - p->tokenbuf)));
  p->tokenpos = 0;
  return 0;
}
