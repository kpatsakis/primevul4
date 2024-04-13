static bool tg3_10_100_only_device(struct tg3 *tp,
				   const struct pci_device_id *ent)
{
	u32 grc_misc_cfg = tr32(GRC_MISC_CFG) & GRC_MISC_CFG_BOARD_ID_MASK;

	if ((tg3_asic_rev(tp) == ASIC_REV_5703 &&
	     (grc_misc_cfg == 0x8000 || grc_misc_cfg == 0x4000)) ||
	    (tp->phy_flags & TG3_PHYFLG_IS_FET))
		return true;

	if (ent->driver_data & TG3_DRV_DATA_FLAG_10_100_ONLY) {
		if (tg3_asic_rev(tp) == ASIC_REV_5705) {
			if (ent->driver_data & TG3_DRV_DATA_FLAG_5705_10_100)
				return true;
		} else {
			return true;
		}
	}

	return false;
}
