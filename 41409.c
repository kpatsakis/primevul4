static void tg3_napi_disable(struct tg3 *tp)
{
	int i;

	for (i = tp->irq_cnt - 1; i >= 0; i--)
		napi_disable(&tp->napi[i].napi);
}
