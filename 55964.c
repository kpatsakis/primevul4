olsr_print_lq_neighbor4(netdissect_options *ndo,
                        const u_char *msg_data, u_int hello_len)
{
    const struct olsr_lq_neighbor4 *lq_neighbor;

    while (hello_len >= sizeof(struct olsr_lq_neighbor4)) {

        lq_neighbor = (const struct olsr_lq_neighbor4 *)msg_data;
        if (!ND_TTEST(*lq_neighbor))
            return (-1);

        ND_PRINT((ndo, "\n\t      neighbor %s, link-quality %.2f%%"
               ", neighbor-link-quality %.2f%%",
               ipaddr_string(ndo, lq_neighbor->neighbor),
               ((double)lq_neighbor->link_quality/2.55),
               ((double)lq_neighbor->neighbor_link_quality/2.55)));

        msg_data += sizeof(struct olsr_lq_neighbor4);
        hello_len -= sizeof(struct olsr_lq_neighbor4);
    }
    return (0);
}
