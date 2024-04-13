static int tg3_ape_otp_read(struct tg3 *tp, u32 offset, u32 *val)
{
	int i, err;
	u32 val2, off = offset * 8;

	err = tg3_nvram_lock(tp);
	if (err)
		return err;

	tg3_ape_write32(tp, TG3_APE_OTP_ADDR, off | APE_OTP_ADDR_CPU_ENABLE);
	tg3_ape_write32(tp, TG3_APE_OTP_CTRL, APE_OTP_CTRL_PROG_EN |
			APE_OTP_CTRL_CMD_RD | APE_OTP_CTRL_START);
	tg3_ape_read32(tp, TG3_APE_OTP_CTRL);
	udelay(10);

	for (i = 0; i < 100; i++) {
		val2 = tg3_ape_read32(tp, TG3_APE_OTP_STATUS);
		if (val2 & APE_OTP_STATUS_CMD_DONE) {
			*val = tg3_ape_read32(tp, TG3_APE_OTP_RD_DATA);
			break;
		}
		udelay(10);
	}

	tg3_ape_write32(tp, TG3_APE_OTP_CTRL, 0);

	tg3_nvram_unlock(tp);
	if (val2 & APE_OTP_STATUS_CMD_DONE)
		return 0;

	return -EBUSY;
}
