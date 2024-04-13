static void print_errors(void)
{
	DPRINT("");
	if (ST0 & ST0_ECE) {
		pr_cont("Recalibrate failed!");
	} else if (ST2 & ST2_CRC) {
		pr_cont("data CRC error");
		tell_sector();
	} else if (ST1 & ST1_CRC) {
		pr_cont("CRC error");
		tell_sector();
	} else if ((ST1 & (ST1_MAM | ST1_ND)) ||
		   (ST2 & ST2_MAM)) {
		if (!probing) {
			pr_cont("sector not found");
			tell_sector();
		} else
			pr_cont("probe failed...");
	} else if (ST2 & ST2_WC) {	/* seek error */
		pr_cont("wrong cylinder");
	} else if (ST2 & ST2_BC) {	/* cylinder marked as bad */
		pr_cont("bad cylinder");
	} else {
		pr_cont("unknown error. ST[0..2] are: 0x%x 0x%x 0x%x",
			ST0, ST1, ST2);
		tell_sector();
	}
	pr_cont("\n");
}
