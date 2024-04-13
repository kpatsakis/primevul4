static u64 tg3_refclk_read(struct tg3 *tp)
{
	u64 stamp = tr32(TG3_EAV_REF_CLCK_LSB);
	return stamp | (u64)tr32(TG3_EAV_REF_CLCK_MSB) << 32;
}
