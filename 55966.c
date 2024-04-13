olsr_print_neighbor(netdissect_options *ndo,
                    const u_char *msg_data, u_int hello_len)
{
    int neighbor;

    ND_PRINT((ndo, "\n\t      neighbor\n\t\t"));
    neighbor = 1;

    while (hello_len >= sizeof(struct in_addr)) {

        if (!ND_TTEST2(*msg_data, sizeof(struct in_addr)))
            return (-1);
        /* print 4 neighbors per line */

        ND_PRINT((ndo, "%s%s", ipaddr_string(ndo, msg_data),
               neighbor % 4 == 0 ? "\n\t\t" : " "));

        msg_data += sizeof(struct in_addr);
        hello_len -= sizeof(struct in_addr);
    }
    return (0);
}
