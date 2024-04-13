static void tg3_refclk_write(struct tg3 *tp, u64 newval)
{
	tw32(TG3_EAV_REF_CLCK_CTL, TG3_EAV_REF_CLCK_CTL_STOP);
	tw32(TG3_EAV_REF_CLCK_LSB, newval & 0xffffffff);
	tw32(TG3_EAV_REF_CLCK_MSB, newval >> 32);
	tw32_f(TG3_EAV_REF_CLCK_CTL, TG3_EAV_REF_CLCK_CTL_RESUME);
}
