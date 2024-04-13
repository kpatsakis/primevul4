resp_print_string_error_integer(netdissect_options *ndo, register const u_char *bp, int length) {
    int length_cur = length, len, ret_len;
    const u_char *bp_ptr;

    /* bp points to the op; skip it */
    SKIP_OPCODE(bp, length_cur);
    bp_ptr = bp;

    /*
     * bp now prints past the (+-;) opcode, so it's pointing to the first
     * character of the string (which could be numeric).
     * +OK\r\n
     * -ERR ...\r\n
     * :02912309\r\n
     *
     * Find the \r\n with FIND_CRLF().
     */
    FIND_CRLF(bp_ptr, length_cur);

    /*
     * bp_ptr points to the \r\n, so bp_ptr - bp is the length of text
     * preceding the \r\n.  That includes the opcode, so don't print
     * that.
     */
    len = (bp_ptr - bp);
    RESP_PRINT_SEGMENT(ndo, bp, len);
    ret_len = 1 /*<opcode>*/ + len /*<string>*/ + 2 /*<CRLF>*/;

    TEST_RET_LEN(ret_len);

trunc:
    return (-1);
}
