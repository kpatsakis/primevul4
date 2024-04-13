static int sclp_ctl_cmdw_supported(unsigned int cmdw)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(sclp_ctl_sccb_wlist); i++) {
		if (cmdw == sclp_ctl_sccb_wlist[i])
			return 1;
	}
	return 0;
}
