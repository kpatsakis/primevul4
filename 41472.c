static void tg3_read_dash_ver(struct tg3 *tp)
{
	int vlen;
	u32 apedata;
	char *fwtype;

	apedata = tg3_ape_read32(tp, TG3_APE_FW_VERSION);

	if (tg3_flag(tp, APE_HAS_NCSI))
		fwtype = "NCSI";
	else if (tp->pdev->device == TG3PCI_DEVICE_TIGON3_5725)
		fwtype = "SMASH";
	else
		fwtype = "DASH";

	vlen = strlen(tp->fw_ver);

	snprintf(&tp->fw_ver[vlen], TG3_VER_SIZE - vlen, " %s v%d.%d.%d.%d",
		 fwtype,
		 (apedata & APE_FW_VERSION_MAJMSK) >> APE_FW_VERSION_MAJSFT,
		 (apedata & APE_FW_VERSION_MINMSK) >> APE_FW_VERSION_MINSFT,
		 (apedata & APE_FW_VERSION_REVMSK) >> APE_FW_VERSION_REVSFT,
		 (apedata & APE_FW_VERSION_BLDMSK));
}
