static int tg3_mdio_init(struct tg3 *tp)
{
	int i;
	u32 reg;
	struct phy_device *phydev;

	if (tg3_flag(tp, 5717_PLUS)) {
		u32 is_serdes;

		tp->phy_addr = tp->pci_fn + 1;

		if (tg3_chip_rev_id(tp) != CHIPREV_ID_5717_A0)
			is_serdes = tr32(SG_DIG_STATUS) & SG_DIG_IS_SERDES;
		else
			is_serdes = tr32(TG3_CPMU_PHY_STRAP) &
				    TG3_CPMU_PHY_STRAP_IS_SERDES;
		if (is_serdes)
			tp->phy_addr += 7;
	} else
		tp->phy_addr = TG3_PHY_MII_ADDR;

	tg3_mdio_start(tp);

	if (!tg3_flag(tp, USE_PHYLIB) || tg3_flag(tp, MDIOBUS_INITED))
		return 0;

	tp->mdio_bus = mdiobus_alloc();
	if (tp->mdio_bus == NULL)
		return -ENOMEM;

	tp->mdio_bus->name     = "tg3 mdio bus";
	snprintf(tp->mdio_bus->id, MII_BUS_ID_SIZE, "%x",
		 (tp->pdev->bus->number << 8) | tp->pdev->devfn);
	tp->mdio_bus->priv     = tp;
	tp->mdio_bus->parent   = &tp->pdev->dev;
	tp->mdio_bus->read     = &tg3_mdio_read;
	tp->mdio_bus->write    = &tg3_mdio_write;
	tp->mdio_bus->reset    = &tg3_mdio_reset;
	tp->mdio_bus->phy_mask = ~(1 << TG3_PHY_MII_ADDR);
	tp->mdio_bus->irq      = &tp->mdio_irq[0];

	for (i = 0; i < PHY_MAX_ADDR; i++)
		tp->mdio_bus->irq[i] = PHY_POLL;

	/* The bus registration will look for all the PHYs on the mdio bus.
	 * Unfortunately, it does not ensure the PHY is powered up before
	 * accessing the PHY ID registers.  A chip reset is the
	 * quickest way to bring the device back to an operational state..
	 */
	if (tg3_readphy(tp, MII_BMCR, &reg) || (reg & BMCR_PDOWN))
		tg3_bmcr_reset(tp);

	i = mdiobus_register(tp->mdio_bus);
	if (i) {
		dev_warn(&tp->pdev->dev, "mdiobus_reg failed (0x%x)\n", i);
		mdiobus_free(tp->mdio_bus);
		return i;
	}

	phydev = tp->mdio_bus->phy_map[TG3_PHY_MII_ADDR];

	if (!phydev || !phydev->drv) {
		dev_warn(&tp->pdev->dev, "No PHY devices\n");
		mdiobus_unregister(tp->mdio_bus);
		mdiobus_free(tp->mdio_bus);
		return -ENODEV;
	}

	switch (phydev->drv->phy_id & phydev->drv->phy_id_mask) {
	case PHY_ID_BCM57780:
		phydev->interface = PHY_INTERFACE_MODE_GMII;
		phydev->dev_flags |= PHY_BRCM_AUTO_PWRDWN_ENABLE;
		break;
	case PHY_ID_BCM50610:
	case PHY_ID_BCM50610M:
		phydev->dev_flags |= PHY_BRCM_CLEAR_RGMII_MODE |
				     PHY_BRCM_RX_REFCLK_UNUSED |
				     PHY_BRCM_DIS_TXCRXC_NOENRGY |
				     PHY_BRCM_AUTO_PWRDWN_ENABLE;
		if (tg3_flag(tp, RGMII_INBAND_DISABLE))
			phydev->dev_flags |= PHY_BRCM_STD_IBND_DISABLE;
		if (tg3_flag(tp, RGMII_EXT_IBND_RX_EN))
			phydev->dev_flags |= PHY_BRCM_EXT_IBND_RX_ENABLE;
		if (tg3_flag(tp, RGMII_EXT_IBND_TX_EN))
			phydev->dev_flags |= PHY_BRCM_EXT_IBND_TX_ENABLE;
		/* fallthru */
	case PHY_ID_RTL8211C:
		phydev->interface = PHY_INTERFACE_MODE_RGMII;
		break;
	case PHY_ID_RTL8201E:
	case PHY_ID_BCMAC131:
		phydev->interface = PHY_INTERFACE_MODE_MII;
		phydev->dev_flags |= PHY_BRCM_AUTO_PWRDWN_ENABLE;
		tp->phy_flags |= TG3_PHYFLG_IS_FET;
		break;
	}

	tg3_flag_set(tp, MDIOBUS_INITED);

	if (tg3_asic_rev(tp) == ASIC_REV_5785)
		tg3_mdio_config_5785(tp);

	return 0;
}
