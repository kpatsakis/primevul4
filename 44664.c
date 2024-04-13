void media_device_unregister(struct media_device *mdev)
{
	struct media_entity *entity;
	struct media_entity *next;

	list_for_each_entry_safe(entity, next, &mdev->entities, list)
		media_device_unregister_entity(entity);

	device_remove_file(&mdev->devnode.dev, &dev_attr_model);
	media_devnode_unregister(&mdev->devnode);
}
