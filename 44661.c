int __must_check media_device_register_entity(struct media_device *mdev,
					      struct media_entity *entity)
{
	/* Warn if we apparently re-register an entity */
	WARN_ON(entity->parent != NULL);
	entity->parent = mdev;

	spin_lock(&mdev->lock);
	if (entity->id == 0)
		entity->id = mdev->entity_id++;
	else
		mdev->entity_id = max(entity->id + 1, mdev->entity_id);
	list_add_tail(&entity->list, &mdev->entities);
	spin_unlock(&mdev->lock);

	return 0;
}
