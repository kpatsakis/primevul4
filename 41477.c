static void tg3_read_otp_ver(struct tg3 *tp)
{
	u32 val, val2;

	if (tg3_asic_rev(tp) != ASIC_REV_5762)
		return;

	if (!tg3_ape_otp_read(tp, OTP_ADDRESS_MAGIC0, &val) &&
	    !tg3_ape_otp_read(tp, OTP_ADDRESS_MAGIC0 + 4, &val2) &&
	    TG3_OTP_MAGIC0_VALID(val)) {
		u64 val64 = (u64) val << 32 | val2;
		u32 ver = 0;
		int i, vlen;

		for (i = 0; i < 7; i++) {
			if ((val64 & 0xff) == 0)
				break;
			ver = val64 & 0xff;
			val64 >>= 8;
		}
		vlen = strlen(tp->fw_ver);
		snprintf(&tp->fw_ver[vlen], TG3_VER_SIZE - vlen, " .%02d", ver);
	}
}
