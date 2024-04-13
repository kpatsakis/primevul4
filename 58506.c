int nfsd_minorversion(u32 minorversion, enum vers_op change)
{
	if (minorversion > NFSD_SUPPORTED_MINOR_VERSION &&
	    change != NFSD_AVAIL)
		return -1;
	switch(change) {
	case NFSD_SET:
		nfsd_supported_minorversions[minorversion] = true;
		nfsd_vers(4, NFSD_SET);
		break;
	case NFSD_CLEAR:
		nfsd_supported_minorversions[minorversion] = false;
		nfsd_adjust_nfsd_versions4();
		break;
	case NFSD_TEST:
		return nfsd_supported_minorversions[minorversion];
	case NFSD_AVAIL:
		return minorversion <= NFSD_SUPPORTED_MINOR_VERSION;
	}
	return 0;
}
