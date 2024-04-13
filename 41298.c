static char *tg3_bus_string(struct tg3 *tp, char *str)
{
	if (tg3_flag(tp, PCI_EXPRESS)) {
		strcpy(str, "PCI Express");
		return str;
	} else if (tg3_flag(tp, PCIX_MODE)) {
		u32 clock_ctrl = tr32(TG3PCI_CLOCK_CTRL) & 0x1f;

		strcpy(str, "PCIX:");

		if ((clock_ctrl == 7) ||
		    ((tr32(GRC_MISC_CFG) & GRC_MISC_CFG_BOARD_ID_MASK) ==
		     GRC_MISC_CFG_BOARD_ID_5704CIOBE))
			strcat(str, "133MHz");
		else if (clock_ctrl == 0)
			strcat(str, "33MHz");
		else if (clock_ctrl == 2)
			strcat(str, "50MHz");
		else if (clock_ctrl == 4)
			strcat(str, "66MHz");
		else if (clock_ctrl == 6)
			strcat(str, "100MHz");
	} else {
		strcpy(str, "PCI:");
		if (tg3_flag(tp, PCI_HIGH_SPEED))
			strcat(str, "66MHz");
		else
			strcat(str, "33MHz");
	}
	if (tg3_flag(tp, PCI_32BIT))
		strcat(str, ":32-bit");
	else
		strcat(str, ":64-bit");
	return str;
}
