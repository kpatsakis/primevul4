nfsd_adjust_nfsd_versions4(void)
{
	unsigned i;

	for (i = 0; i <= NFSD_SUPPORTED_MINOR_VERSION; i++) {
		if (nfsd_supported_minorversions[i])
			return;
	}
	nfsd_vers(4, NFSD_CLEAR);
}
