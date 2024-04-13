static int hiddev_release(struct inode * inode, struct file * file)
{
	struct hiddev_list *list = file->private_data;
	unsigned long flags;

	spin_lock_irqsave(&list->hiddev->list_lock, flags);
	list_del(&list->node);
	spin_unlock_irqrestore(&list->hiddev->list_lock, flags);

	mutex_lock(&list->hiddev->existancelock);
	if (!--list->hiddev->open) {
		if (list->hiddev->exist) {
			usbhid_close(list->hiddev->hid);
			usbhid_put_power(list->hiddev->hid);
		} else {
			mutex_unlock(&list->hiddev->existancelock);
			kfree(list->hiddev);
			vfree(list);
			return 0;
		}
	}

	mutex_unlock(&list->hiddev->existancelock);
	vfree(list);

	return 0;
}
