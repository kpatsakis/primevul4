static void tg3_read_fw_ver(struct tg3 *tp)
{
	u32 val;
	bool vpd_vers = false;

	if (tp->fw_ver[0] != 0)
		vpd_vers = true;

	if (tg3_flag(tp, NO_NVRAM)) {
		strcat(tp->fw_ver, "sb");
		tg3_read_otp_ver(tp);
		return;
	}

	if (tg3_nvram_read(tp, 0, &val))
		return;

	if (val == TG3_EEPROM_MAGIC)
		tg3_read_bc_ver(tp);
	else if ((val & TG3_EEPROM_MAGIC_FW_MSK) == TG3_EEPROM_MAGIC_FW)
		tg3_read_sb_ver(tp, val);
	else if ((val & TG3_EEPROM_MAGIC_HW_MSK) == TG3_EEPROM_MAGIC_HW)
		tg3_read_hwsb_ver(tp);

	if (tg3_flag(tp, ENABLE_ASF)) {
		if (tg3_flag(tp, ENABLE_APE)) {
			tg3_probe_ncsi(tp);
			if (!vpd_vers)
				tg3_read_dash_ver(tp);
		} else if (!vpd_vers) {
			tg3_read_mgmtfw_ver(tp);
		}
	}

	tp->fw_ver[TG3_VER_SIZE - 1] = 0;
}
