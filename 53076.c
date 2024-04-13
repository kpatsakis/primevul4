static s32 atl2_phy_commit(struct atl2_hw *hw)
{
	s32 ret_val;
	u16 phy_data;

	phy_data = MII_CR_RESET | MII_CR_AUTO_NEG_EN | MII_CR_RESTART_AUTO_NEG;
	ret_val = atl2_write_phy_reg(hw, MII_BMCR, phy_data);
	if (ret_val) {
		u32 val;
		int i;
		/* pcie serdes link may be down ! */
		for (i = 0; i < 25; i++) {
			msleep(1);
			val = ATL2_READ_REG(hw, REG_MDIO_CTRL);
			if (!(val & (MDIO_START | MDIO_BUSY)))
				break;
		}

		if (0 != (val & (MDIO_START | MDIO_BUSY))) {
			printk(KERN_ERR "atl2: PCIe link down for at least 25ms !\n");
			return ret_val;
		}
	}
	return 0;
}
