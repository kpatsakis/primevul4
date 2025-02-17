static void tg3_enable_nvram_access(struct tg3 *tp)
{
	if (tg3_flag(tp, 5750_PLUS) && !tg3_flag(tp, PROTECTED_NVRAM)) {
		u32 nvaccess = tr32(NVRAM_ACCESS);

		tw32(NVRAM_ACCESS, nvaccess | ACCESS_ENABLE);
	}
}
