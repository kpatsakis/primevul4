resp_print_bulk_array(netdissect_options *ndo, register const u_char *bp, int length) {
    u_int length_cur = length;
    int array_len, i, ret_len;

    /* bp points to the op; skip it */
    SKIP_OPCODE(bp, length_cur);

    /* <array_length>\r\n */
    GET_LENGTH(ndo, length_cur, bp, array_len);

    if (array_len > 0) {
        /* non empty array */
        for (i = 0; i < array_len; i++) {
            ret_len = resp_parse(ndo, bp, length_cur);

            TEST_RET_LEN_NORETURN(ret_len);

            bp += ret_len;
            length_cur -= ret_len;
        }
    } else {
        /* empty, null, truncated, or invalid */
        switch(array_len) {
            case 0:     resp_print_empty(ndo);            break;
            case (-1):  resp_print_null(ndo);             break;
            case (-2):  goto trunc;
            case (-3):  resp_print_length_too_large(ndo); break;
            case (-4):  resp_print_length_negative(ndo);  break;
            default:    resp_print_invalid(ndo);          break;
        }
    }

    return (length - length_cur);

trunc:
    return (-1);
}
