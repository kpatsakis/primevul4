nfsreply_print(netdissect_options *ndo,
               register const u_char *bp, u_int length,
               register const u_char *bp2)
{
	register const struct sunrpc_msg *rp;
	char srcid[20], dstid[20];	/*fits 32bit*/

	nfserr = 0;		/* assume no error */
	rp = (const struct sunrpc_msg *)bp;

	ND_TCHECK(rp->rm_xid);
	if (!ndo->ndo_nflag) {
		strlcpy(srcid, "nfs", sizeof(srcid));
		snprintf(dstid, sizeof(dstid), "%u",
		    EXTRACT_32BITS(&rp->rm_xid));
	} else {
		snprintf(srcid, sizeof(srcid), "%u", NFS_PORT);
		snprintf(dstid, sizeof(dstid), "%u",
		    EXTRACT_32BITS(&rp->rm_xid));
	}
	print_nfsaddr(ndo, bp2, srcid, dstid);

	nfsreply_print_noaddr(ndo, bp, length, bp2);
	return;

trunc:
	if (!nfserr)
		ND_PRINT((ndo, "%s", tstr));
}
