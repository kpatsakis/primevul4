static void hid_free_dynids(struct hid_driver *hdrv)
{
	struct hid_dynid *dynid, *n;

	spin_lock(&hdrv->dyn_lock);
	list_for_each_entry_safe(dynid, n, &hdrv->dyn_list, list) {
		list_del(&dynid->list);
		kfree(dynid);
	}
	spin_unlock(&hdrv->dyn_lock);
}
