static int atl2_set_eeprom(struct net_device *netdev,
	struct ethtool_eeprom *eeprom, u8 *bytes)
{
	struct atl2_adapter *adapter = netdev_priv(netdev);
	struct atl2_hw *hw = &adapter->hw;
	u32 *eeprom_buff;
	u32 *ptr;
	int max_len, first_dword, last_dword, ret_val = 0;
	int i;

	if (eeprom->len == 0)
		return -EOPNOTSUPP;

	if (eeprom->magic != (hw->vendor_id | (hw->device_id << 16)))
		return -EFAULT;

	max_len = 512;

	first_dword = eeprom->offset >> 2;
	last_dword = (eeprom->offset + eeprom->len - 1) >> 2;
	eeprom_buff = kmalloc(max_len, GFP_KERNEL);
	if (!eeprom_buff)
		return -ENOMEM;

	ptr = eeprom_buff;

	if (eeprom->offset & 3) {
		/* need read/modify/write of first changed EEPROM word */
		/* only the second byte of the word is being modified */
		if (!atl2_read_eeprom(hw, first_dword*4, &(eeprom_buff[0]))) {
			ret_val = -EIO;
			goto out;
		}
		ptr++;
	}
	if (((eeprom->offset + eeprom->len) & 3)) {
		/*
		 * need read/modify/write of last changed EEPROM word
		 * only the first byte of the word is being modified
		 */
		if (!atl2_read_eeprom(hw, last_dword * 4,
					&(eeprom_buff[last_dword - first_dword]))) {
			ret_val = -EIO;
			goto out;
		}
	}

	/* Device's eeprom is always little-endian, word addressable */
	memcpy(ptr, bytes, eeprom->len);

	for (i = 0; i < last_dword - first_dword + 1; i++) {
		if (!atl2_write_eeprom(hw, ((first_dword+i)*4), eeprom_buff[i])) {
			ret_val = -EIO;
			goto out;
		}
	}
 out:
	kfree(eeprom_buff);
	return ret_val;
}
