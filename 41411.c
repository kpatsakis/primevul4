static void tg3_napi_init(struct tg3 *tp)
{
	int i;

	netif_napi_add(tp->dev, &tp->napi[0].napi, tg3_poll, 64);
	for (i = 1; i < tp->irq_cnt; i++)
		netif_napi_add(tp->dev, &tp->napi[i].napi, tg3_poll_msix, 64);
}
