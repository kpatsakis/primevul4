static int brcmf_get_first_free_bsscfgidx(struct brcmf_pub *drvr)
{
	int bsscfgidx;

	for (bsscfgidx = 0; bsscfgidx < BRCMF_MAX_IFS; bsscfgidx++) {
		/* bsscfgidx 1 is reserved for legacy P2P */
		if (bsscfgidx == 1)
			continue;
		if (!drvr->iflist[bsscfgidx])
			return bsscfgidx;
	}

	return -ENOMEM;
}
