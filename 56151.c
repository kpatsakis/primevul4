wb_rreq(netdissect_options *ndo,
        const struct pkt_rreq *rreq, u_int len)
{
	ND_PRINT((ndo, " wb-rreq:"));
	if (len < sizeof(*rreq) || !ND_TTEST(*rreq))
		return (-1);

	ND_PRINT((ndo, " please repair %s %s:%u<%u:%u>",
	       ipaddr_string(ndo, &rreq->pr_id),
	       ipaddr_string(ndo, &rreq->pr_page.p_sid),
	       EXTRACT_32BITS(&rreq->pr_page.p_uid),
	       EXTRACT_32BITS(&rreq->pr_sseq),
	       EXTRACT_32BITS(&rreq->pr_eseq)));
	return (0);
}
