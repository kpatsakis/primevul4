static void tg3_napi_fini(struct tg3 *tp)
{
	int i;

	for (i = 0; i < tp->irq_cnt; i++)
		netif_napi_del(&tp->napi[i].napi);
}
