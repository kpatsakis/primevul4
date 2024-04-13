static int tg3_get_device_address(struct tg3 *tp)
{
	struct net_device *dev = tp->dev;
	u32 hi, lo, mac_offset;
	int addr_ok = 0;
	int err;

#ifdef CONFIG_SPARC
	if (!tg3_get_macaddr_sparc(tp))
		return 0;
#endif

	if (tg3_flag(tp, IS_SSB_CORE)) {
		err = ssb_gige_get_macaddr(tp->pdev, &dev->dev_addr[0]);
		if (!err && is_valid_ether_addr(&dev->dev_addr[0]))
			return 0;
	}

	mac_offset = 0x7c;
	if (tg3_asic_rev(tp) == ASIC_REV_5704 ||
	    tg3_flag(tp, 5780_CLASS)) {
		if (tr32(TG3PCI_DUAL_MAC_CTRL) & DUAL_MAC_CTRL_ID)
			mac_offset = 0xcc;
		if (tg3_nvram_lock(tp))
			tw32_f(NVRAM_CMD, NVRAM_CMD_RESET);
		else
			tg3_nvram_unlock(tp);
	} else if (tg3_flag(tp, 5717_PLUS)) {
		if (tp->pci_fn & 1)
			mac_offset = 0xcc;
		if (tp->pci_fn > 1)
			mac_offset += 0x18c;
	} else if (tg3_asic_rev(tp) == ASIC_REV_5906)
		mac_offset = 0x10;

	/* First try to get it from MAC address mailbox. */
	tg3_read_mem(tp, NIC_SRAM_MAC_ADDR_HIGH_MBOX, &hi);
	if ((hi >> 16) == 0x484b) {
		dev->dev_addr[0] = (hi >>  8) & 0xff;
		dev->dev_addr[1] = (hi >>  0) & 0xff;

		tg3_read_mem(tp, NIC_SRAM_MAC_ADDR_LOW_MBOX, &lo);
		dev->dev_addr[2] = (lo >> 24) & 0xff;
		dev->dev_addr[3] = (lo >> 16) & 0xff;
		dev->dev_addr[4] = (lo >>  8) & 0xff;
		dev->dev_addr[5] = (lo >>  0) & 0xff;

		/* Some old bootcode may report a 0 MAC address in SRAM */
		addr_ok = is_valid_ether_addr(&dev->dev_addr[0]);
	}
	if (!addr_ok) {
		/* Next, try NVRAM. */
		if (!tg3_flag(tp, NO_NVRAM) &&
		    !tg3_nvram_read_be32(tp, mac_offset + 0, &hi) &&
		    !tg3_nvram_read_be32(tp, mac_offset + 4, &lo)) {
			memcpy(&dev->dev_addr[0], ((char *)&hi) + 2, 2);
			memcpy(&dev->dev_addr[2], (char *)&lo, sizeof(lo));
		}
		/* Finally just fetch it out of the MAC control regs. */
		else {
			hi = tr32(MAC_ADDR_0_HIGH);
			lo = tr32(MAC_ADDR_0_LOW);

			dev->dev_addr[5] = lo & 0xff;
			dev->dev_addr[4] = (lo >> 8) & 0xff;
			dev->dev_addr[3] = (lo >> 16) & 0xff;
			dev->dev_addr[2] = (lo >> 24) & 0xff;
			dev->dev_addr[1] = hi & 0xff;
			dev->dev_addr[0] = (hi >> 8) & 0xff;
		}
	}

	if (!is_valid_ether_addr(&dev->dev_addr[0])) {
#ifdef CONFIG_SPARC
		if (!tg3_get_default_macaddr_sparc(tp))
			return 0;
#endif
		return -EINVAL;
	}
	return 0;
}
