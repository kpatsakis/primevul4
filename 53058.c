static s32 atl2_get_speed_and_duplex(struct atl2_hw *hw, u16 *speed,
	u16 *duplex)
{
	s32 ret_val;
	u16 phy_data;

	/* Read PHY Specific Status Register (17) */
	ret_val = atl2_read_phy_reg(hw, MII_ATLX_PSSR, &phy_data);
	if (ret_val)
		return ret_val;

	if (!(phy_data & MII_ATLX_PSSR_SPD_DPLX_RESOLVED))
		return ATLX_ERR_PHY_RES;

	switch (phy_data & MII_ATLX_PSSR_SPEED) {
	case MII_ATLX_PSSR_100MBS:
		*speed = SPEED_100;
		break;
	case MII_ATLX_PSSR_10MBS:
		*speed = SPEED_10;
		break;
	default:
		return ATLX_ERR_PHY_SPEED;
	}

	if (phy_data & MII_ATLX_PSSR_DPLX)
		*duplex = FULL_DUPLEX;
	else
		*duplex = HALF_DUPLEX;

	return 0;
}
