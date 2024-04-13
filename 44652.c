static struct media_entity *find_entity(struct media_device *mdev, u32 id)
{
	struct media_entity *entity;
	int next = id & MEDIA_ENT_ID_FLAG_NEXT;

	id &= ~MEDIA_ENT_ID_FLAG_NEXT;

	spin_lock(&mdev->lock);

	media_device_for_each_entity(entity, mdev) {
		if ((entity->id == id && !next) ||
		    (entity->id > id && next)) {
			spin_unlock(&mdev->lock);
			return entity;
		}
	}

	spin_unlock(&mdev->lock);

	return NULL;
}
