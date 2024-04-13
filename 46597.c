static void export_array(struct mddev *mddev)
{
	struct md_rdev *rdev;

	while (!list_empty(&mddev->disks)) {
		rdev = list_first_entry(&mddev->disks, struct md_rdev,
					same_set);
		md_kick_rdev_from_array(rdev);
	}
	mddev->raid_disks = 0;
	mddev->major_version = 0;
}
