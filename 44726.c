static char __init get_fdc_version(void)
{
	int r;

	output_byte(FD_DUMPREGS);	/* 82072 and better know DUMPREGS */
	if (FDCS->reset)
		return FDC_NONE;
	r = result();
	if (r <= 0x00)
		return FDC_NONE;	/* No FDC present ??? */
	if ((r == 1) && (reply_buffer[0] == 0x80)) {
		pr_info("FDC %d is an 8272A\n", fdc);
		return FDC_8272A;	/* 8272a/765 don't know DUMPREGS */
	}
	if (r != 10) {
		pr_info("FDC %d init: DUMPREGS: unexpected return of %d bytes.\n",
			fdc, r);
		return FDC_UNKNOWN;
	}

	if (!fdc_configure()) {
		pr_info("FDC %d is an 82072\n", fdc);
		return FDC_82072;	/* 82072 doesn't know CONFIGURE */
	}

	output_byte(FD_PERPENDICULAR);
	if (need_more_output() == MORE_OUTPUT) {
		output_byte(0);
	} else {
		pr_info("FDC %d is an 82072A\n", fdc);
		return FDC_82072A;	/* 82072A as found on Sparcs. */
	}

	output_byte(FD_UNLOCK);
	r = result();
	if ((r == 1) && (reply_buffer[0] == 0x80)) {
		pr_info("FDC %d is a pre-1991 82077\n", fdc);
		return FDC_82077_ORIG;	/* Pre-1991 82077, doesn't know
					 * LOCK/UNLOCK */
	}
	if ((r != 1) || (reply_buffer[0] != 0x00)) {
		pr_info("FDC %d init: UNLOCK: unexpected return of %d bytes.\n",
			fdc, r);
		return FDC_UNKNOWN;
	}
	output_byte(FD_PARTID);
	r = result();
	if (r != 1) {
		pr_info("FDC %d init: PARTID: unexpected return of %d bytes.\n",
			fdc, r);
		return FDC_UNKNOWN;
	}
	if (reply_buffer[0] == 0x80) {
		pr_info("FDC %d is a post-1991 82077\n", fdc);
		return FDC_82077;	/* Revised 82077AA passes all the tests */
	}
	switch (reply_buffer[0] >> 5) {
	case 0x0:
		/* Either a 82078-1 or a 82078SL running at 5Volt */
		pr_info("FDC %d is an 82078.\n", fdc);
		return FDC_82078;
	case 0x1:
		pr_info("FDC %d is a 44pin 82078\n", fdc);
		return FDC_82078;
	case 0x2:
		pr_info("FDC %d is a S82078B\n", fdc);
		return FDC_S82078B;
	case 0x3:
		pr_info("FDC %d is a National Semiconductor PC87306\n", fdc);
		return FDC_87306;
	default:
		pr_info("FDC %d init: 82078 variant with unknown PARTID=%d.\n",
			fdc, reply_buffer[0] >> 5);
		return FDC_82078_UNKN;
	}
}				/* get_fdc_version */
