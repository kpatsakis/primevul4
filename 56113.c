print_dns_label(netdissect_options *ndo,
                const u_char *cp, u_int max_length, int print)
{
    u_int length = 0;
    while (length < max_length) {
        u_int lab_length = cp[length++];
        if (lab_length == 0)
            return (int)length;
        if (length > 1 && print)
            safeputchar(ndo, '.');
        if (length+lab_length > max_length) {
            if (print)
                safeputs(ndo, cp+length, max_length-length);
            break;
        }
        if (print)
            safeputs(ndo, cp+length, lab_length);
        length += lab_length;
    }
    if (print)
        ND_PRINT((ndo, "[|DNS]"));
    return -1;
}
