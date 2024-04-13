void nfsd_reset_versions(void)
{
	int i;

	for (i = 0; i < NFSD_NRVERS; i++)
		if (nfsd_vers(i, NFSD_TEST))
			return;

	for (i = 0; i < NFSD_NRVERS; i++)
		if (i != 4)
			nfsd_vers(i, NFSD_SET);
		else {
			int minor = 0;
			while (nfsd_minorversion(minor, NFSD_SET) >= 0)
				minor++;
		}
}
