static void tg3_carrier_off(struct tg3 *tp)
{
	netif_carrier_off(tp->dev);
	tp->link_up = false;
}
