print_sattr3(netdissect_options *ndo,
             const struct nfsv3_sattr *sa3, int verbose)
{
	if (sa3->sa_modeset)
		ND_PRINT((ndo, " mode %o", sa3->sa_mode));
	if (sa3->sa_uidset)
		ND_PRINT((ndo, " uid %u", sa3->sa_uid));
	if (sa3->sa_gidset)
		ND_PRINT((ndo, " gid %u", sa3->sa_gid));
	if (verbose > 1) {
		if (sa3->sa_atimetype == NFSV3SATTRTIME_TOCLIENT)
			ND_PRINT((ndo, " atime %u.%06u", sa3->sa_atime.nfsv3_sec,
			       sa3->sa_atime.nfsv3_nsec));
		if (sa3->sa_mtimetype == NFSV3SATTRTIME_TOCLIENT)
			ND_PRINT((ndo, " mtime %u.%06u", sa3->sa_mtime.nfsv3_sec,
			       sa3->sa_mtime.nfsv3_nsec));
	}
}
