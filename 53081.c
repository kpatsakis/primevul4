static s32 atl2_read_mac_addr(struct atl2_hw *hw)
{
	if (get_permanent_address(hw)) {
		/* for test */
		/* FIXME: shouldn't we use eth_random_addr() here? */
		hw->perm_mac_addr[0] = 0x00;
		hw->perm_mac_addr[1] = 0x13;
		hw->perm_mac_addr[2] = 0x74;
		hw->perm_mac_addr[3] = 0x00;
		hw->perm_mac_addr[4] = 0x5c;
		hw->perm_mac_addr[5] = 0x38;
	}

	memcpy(hw->mac_addr, hw->perm_mac_addr, ETH_ALEN);

	return 0;
}
