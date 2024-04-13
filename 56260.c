resp_print_error(netdissect_options *ndo, register const u_char *bp, int length) {
    return resp_print_string_error_integer(ndo, bp, length);
}
