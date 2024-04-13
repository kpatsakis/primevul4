handle_reassoc_response(netdissect_options *ndo,
                        const u_char *p, u_int length)
{
	/* Same as a Association Reponse */
	return handle_assoc_response(ndo, p, length);
}
