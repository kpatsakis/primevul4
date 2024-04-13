static void tg3_mdio_fini(struct tg3 *tp)
{
	if (tg3_flag(tp, MDIOBUS_INITED)) {
		tg3_flag_clear(tp, MDIOBUS_INITED);
		mdiobus_unregister(tp->mdio_bus);
		mdiobus_free(tp->mdio_bus);
	}
}
