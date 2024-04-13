static int find_ci(const struct atm_vcc *vcc, short *vpi, int *vci)
{
	static short p;        /* poor man's per-device cache */
	static int c;
	short old_p;
	int old_c;
	int err;

	if (*vpi != ATM_VPI_ANY && *vci != ATM_VCI_ANY) {
		err = check_ci(vcc, *vpi, *vci);
		return err;
	}
	/* last scan may have left values out of bounds for current device */
	if (*vpi != ATM_VPI_ANY)
		p = *vpi;
	else if (p >= 1 << vcc->dev->ci_range.vpi_bits)
		p = 0;
	if (*vci != ATM_VCI_ANY)
		c = *vci;
	else if (c < ATM_NOT_RSV_VCI || c >= 1 << vcc->dev->ci_range.vci_bits)
			c = ATM_NOT_RSV_VCI;
	old_p = p;
	old_c = c;
	do {
		if (!check_ci(vcc, p, c)) {
			*vpi = p;
			*vci = c;
			return 0;
		}
		if (*vci == ATM_VCI_ANY) {
			c++;
			if (c >= 1 << vcc->dev->ci_range.vci_bits)
				c = ATM_NOT_RSV_VCI;
		}
		if ((c == ATM_NOT_RSV_VCI || *vci != ATM_VCI_ANY) &&
		    *vpi == ATM_VPI_ANY) {
			p++;
			if (p >= 1 << vcc->dev->ci_range.vpi_bits)
				p = 0;
		}
	} while (old_p != p || old_c != c);
	return -EADDRINUSE;
}
