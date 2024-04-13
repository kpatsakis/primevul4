olsr_print_lq_neighbor6(netdissect_options *ndo,
                        const u_char *msg_data, u_int hello_len)
{
    const struct olsr_lq_neighbor6 *lq_neighbor;

    while (hello_len >= sizeof(struct olsr_lq_neighbor6)) {

        lq_neighbor = (const struct olsr_lq_neighbor6 *)msg_data;
        if (!ND_TTEST(*lq_neighbor))
            return (-1);

        ND_PRINT((ndo, "\n\t      neighbor %s, link-quality %.2f%%"
               ", neighbor-link-quality %.2f%%",
               ip6addr_string(ndo, lq_neighbor->neighbor),
               ((double)lq_neighbor->link_quality/2.55),
               ((double)lq_neighbor->neighbor_link_quality/2.55)));

        msg_data += sizeof(struct olsr_lq_neighbor6);
        hello_len -= sizeof(struct olsr_lq_neighbor6);
    }
    return (0);
}
