static int tg3_test_nvram(struct tg3 *tp)
{
	u32 csum, magic, len;
	__be32 *buf;
	int i, j, k, err = 0, size;

	if (tg3_flag(tp, NO_NVRAM))
		return 0;

	if (tg3_nvram_read(tp, 0, &magic) != 0)
		return -EIO;

	if (magic == TG3_EEPROM_MAGIC)
		size = NVRAM_TEST_SIZE;
	else if ((magic & TG3_EEPROM_MAGIC_FW_MSK) == TG3_EEPROM_MAGIC_FW) {
		if ((magic & TG3_EEPROM_SB_FORMAT_MASK) ==
		    TG3_EEPROM_SB_FORMAT_1) {
			switch (magic & TG3_EEPROM_SB_REVISION_MASK) {
			case TG3_EEPROM_SB_REVISION_0:
				size = NVRAM_SELFBOOT_FORMAT1_0_SIZE;
				break;
			case TG3_EEPROM_SB_REVISION_2:
				size = NVRAM_SELFBOOT_FORMAT1_2_SIZE;
				break;
			case TG3_EEPROM_SB_REVISION_3:
				size = NVRAM_SELFBOOT_FORMAT1_3_SIZE;
				break;
			case TG3_EEPROM_SB_REVISION_4:
				size = NVRAM_SELFBOOT_FORMAT1_4_SIZE;
				break;
			case TG3_EEPROM_SB_REVISION_5:
				size = NVRAM_SELFBOOT_FORMAT1_5_SIZE;
				break;
			case TG3_EEPROM_SB_REVISION_6:
				size = NVRAM_SELFBOOT_FORMAT1_6_SIZE;
				break;
			default:
				return -EIO;
			}
		} else
			return 0;
	} else if ((magic & TG3_EEPROM_MAGIC_HW_MSK) == TG3_EEPROM_MAGIC_HW)
		size = NVRAM_SELFBOOT_HW_SIZE;
	else
		return -EIO;

	buf = kmalloc(size, GFP_KERNEL);
	if (buf == NULL)
		return -ENOMEM;

	err = -EIO;
	for (i = 0, j = 0; i < size; i += 4, j++) {
		err = tg3_nvram_read_be32(tp, i, &buf[j]);
		if (err)
			break;
	}
	if (i < size)
		goto out;

	/* Selfboot format */
	magic = be32_to_cpu(buf[0]);
	if ((magic & TG3_EEPROM_MAGIC_FW_MSK) ==
	    TG3_EEPROM_MAGIC_FW) {
		u8 *buf8 = (u8 *) buf, csum8 = 0;

		if ((magic & TG3_EEPROM_SB_REVISION_MASK) ==
		    TG3_EEPROM_SB_REVISION_2) {
			/* For rev 2, the csum doesn't include the MBA. */
			for (i = 0; i < TG3_EEPROM_SB_F1R2_MBA_OFF; i++)
				csum8 += buf8[i];
			for (i = TG3_EEPROM_SB_F1R2_MBA_OFF + 4; i < size; i++)
				csum8 += buf8[i];
		} else {
			for (i = 0; i < size; i++)
				csum8 += buf8[i];
		}

		if (csum8 == 0) {
			err = 0;
			goto out;
		}

		err = -EIO;
		goto out;
	}

	if ((magic & TG3_EEPROM_MAGIC_HW_MSK) ==
	    TG3_EEPROM_MAGIC_HW) {
		u8 data[NVRAM_SELFBOOT_DATA_SIZE];
		u8 parity[NVRAM_SELFBOOT_DATA_SIZE];
		u8 *buf8 = (u8 *) buf;

		/* Separate the parity bits and the data bytes.  */
		for (i = 0, j = 0, k = 0; i < NVRAM_SELFBOOT_HW_SIZE; i++) {
			if ((i == 0) || (i == 8)) {
				int l;
				u8 msk;

				for (l = 0, msk = 0x80; l < 7; l++, msk >>= 1)
					parity[k++] = buf8[i] & msk;
				i++;
			} else if (i == 16) {
				int l;
				u8 msk;

				for (l = 0, msk = 0x20; l < 6; l++, msk >>= 1)
					parity[k++] = buf8[i] & msk;
				i++;

				for (l = 0, msk = 0x80; l < 8; l++, msk >>= 1)
					parity[k++] = buf8[i] & msk;
				i++;
			}
			data[j++] = buf8[i];
		}

		err = -EIO;
		for (i = 0; i < NVRAM_SELFBOOT_DATA_SIZE; i++) {
			u8 hw8 = hweight8(data[i]);

			if ((hw8 & 0x1) && parity[i])
				goto out;
			else if (!(hw8 & 0x1) && !parity[i])
				goto out;
		}
		err = 0;
		goto out;
	}

	err = -EIO;

	/* Bootstrap checksum at offset 0x10 */
	csum = calc_crc((unsigned char *) buf, 0x10);
	if (csum != le32_to_cpu(buf[0x10/4]))
		goto out;

	/* Manufacturing block starts at offset 0x74, checksum at 0xfc */
	csum = calc_crc((unsigned char *) &buf[0x74/4], 0x88);
	if (csum != le32_to_cpu(buf[0xfc/4]))
		goto out;

	kfree(buf);

	buf = tg3_vpd_readblock(tp, &len);
	if (!buf)
		return -ENOMEM;

	i = pci_vpd_find_tag((u8 *)buf, 0, len, PCI_VPD_LRDT_RO_DATA);
	if (i > 0) {
		j = pci_vpd_lrdt_size(&((u8 *)buf)[i]);
		if (j < 0)
			goto out;

		if (i + PCI_VPD_LRDT_TAG_SIZE + j > len)
			goto out;

		i += PCI_VPD_LRDT_TAG_SIZE;
		j = pci_vpd_find_info_keyword((u8 *)buf, i, j,
					      PCI_VPD_RO_KEYWORD_CHKSUM);
		if (j > 0) {
			u8 csum8 = 0;

			j += PCI_VPD_INFO_FLD_HDR_SIZE;

			for (i = 0; i <= j; i++)
				csum8 += ((u8 *)buf)[i];

			if (csum8)
				goto out;
		}
	}

	err = 0;

out:
	kfree(buf);
	return err;
}
