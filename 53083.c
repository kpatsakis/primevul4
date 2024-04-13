static s32 atl2_read_phy_reg(struct atl2_hw *hw, u16 reg_addr, u16 *phy_data)
{
	u32 val;
	int i;

	val = ((u32)(reg_addr & MDIO_REG_ADDR_MASK)) << MDIO_REG_ADDR_SHIFT |
		MDIO_START |
		MDIO_SUP_PREAMBLE |
		MDIO_RW |
		MDIO_CLK_25_4 << MDIO_CLK_SEL_SHIFT;
	ATL2_WRITE_REG(hw, REG_MDIO_CTRL, val);

	wmb();

	for (i = 0; i < MDIO_WAIT_TIMES; i++) {
		udelay(2);
		val = ATL2_READ_REG(hw, REG_MDIO_CTRL);
		if (!(val & (MDIO_START | MDIO_BUSY)))
			break;
		wmb();
	}
	if (!(val & (MDIO_START | MDIO_BUSY))) {
		*phy_data = (u16)val;
		return 0;
	}

	return ATLX_ERR_PHY;
}
