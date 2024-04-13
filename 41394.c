static struct subsys_tbl_ent *tg3_lookup_by_subsys(struct tg3 *tp)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(subsys_id_to_phy_id); i++) {
		if ((subsys_id_to_phy_id[i].subsys_vendor ==
		     tp->pdev->subsystem_vendor) &&
		    (subsys_id_to_phy_id[i].subsys_devid ==
		     tp->pdev->subsystem_device))
			return &subsys_id_to_phy_id[i];
	}
	return NULL;
}
