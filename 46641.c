static int __init md_init(void)
{
	int ret = -ENOMEM;

	md_wq = alloc_workqueue("md", WQ_MEM_RECLAIM, 0);
	if (!md_wq)
		goto err_wq;

	md_misc_wq = alloc_workqueue("md_misc", 0, 0);
	if (!md_misc_wq)
		goto err_misc_wq;

	if ((ret = register_blkdev(MD_MAJOR, "md")) < 0)
		goto err_md;

	if ((ret = register_blkdev(0, "mdp")) < 0)
		goto err_mdp;
	mdp_major = ret;

	blk_register_region(MKDEV(MD_MAJOR, 0), 512, THIS_MODULE,
			    md_probe, NULL, NULL);
	blk_register_region(MKDEV(mdp_major, 0), 1UL<<MINORBITS, THIS_MODULE,
			    md_probe, NULL, NULL);

	register_reboot_notifier(&md_notifier);
	raid_table_header = register_sysctl_table(raid_root_table);

	md_geninit();
	return 0;

err_mdp:
	unregister_blkdev(MD_MAJOR, "md");
err_md:
	destroy_workqueue(md_misc_wq);
err_misc_wq:
	destroy_workqueue(md_wq);
err_wq:
	return ret;
}
