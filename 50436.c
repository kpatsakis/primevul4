 static bool ion_handle_validate(struct ion_client *client,
 				struct ion_handle *handle)
 {
	WARN_ON(!mutex_is_locked(&client->lock));
	return idr_find(&client->idr, handle->id) == handle;
}
