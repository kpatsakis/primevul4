static void tg3_nvram_unlock(struct tg3 *tp)
{
	if (tg3_flag(tp, NVRAM)) {
		if (tp->nvram_lock_cnt > 0)
			tp->nvram_lock_cnt--;
		if (tp->nvram_lock_cnt == 0)
			tw32_f(NVRAM_SWARB, SWARB_REQ_CLR1);
	}
}
