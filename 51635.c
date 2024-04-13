static ssize_t hiddev_read(struct file * file, char __user * buffer, size_t count, loff_t *ppos)
{
	DEFINE_WAIT(wait);
	struct hiddev_list *list = file->private_data;
	int event_size;
	int retval;

	event_size = ((list->flags & HIDDEV_FLAG_UREF) != 0) ?
		sizeof(struct hiddev_usage_ref) : sizeof(struct hiddev_event);

	if (count < event_size)
		return 0;

	/* lock against other threads */
	retval = mutex_lock_interruptible(&list->thread_lock);
	if (retval)
		return -ERESTARTSYS;

	while (retval == 0) {
		if (list->head == list->tail) {
			prepare_to_wait(&list->hiddev->wait, &wait, TASK_INTERRUPTIBLE);

			while (list->head == list->tail) {
				if (signal_pending(current)) {
					retval = -ERESTARTSYS;
					break;
				}
				if (!list->hiddev->exist) {
					retval = -EIO;
					break;
				}
				if (file->f_flags & O_NONBLOCK) {
					retval = -EAGAIN;
					break;
				}

				/* let O_NONBLOCK tasks run */
				mutex_unlock(&list->thread_lock);
				schedule();
				if (mutex_lock_interruptible(&list->thread_lock)) {
					finish_wait(&list->hiddev->wait, &wait);
					return -EINTR;
				}
				set_current_state(TASK_INTERRUPTIBLE);
			}
			finish_wait(&list->hiddev->wait, &wait);

		}

		if (retval) {
			mutex_unlock(&list->thread_lock);
			return retval;
		}


		while (list->head != list->tail &&
		       retval + event_size <= count) {
			if ((list->flags & HIDDEV_FLAG_UREF) == 0) {
				if (list->buffer[list->tail].field_index != HID_FIELD_INDEX_NONE) {
					struct hiddev_event event;

					event.hid = list->buffer[list->tail].usage_code;
					event.value = list->buffer[list->tail].value;
					if (copy_to_user(buffer + retval, &event, sizeof(struct hiddev_event))) {
						mutex_unlock(&list->thread_lock);
						return -EFAULT;
					}
					retval += sizeof(struct hiddev_event);
				}
			} else {
				if (list->buffer[list->tail].field_index != HID_FIELD_INDEX_NONE ||
				    (list->flags & HIDDEV_FLAG_REPORT) != 0) {

					if (copy_to_user(buffer + retval, list->buffer + list->tail, sizeof(struct hiddev_usage_ref))) {
						mutex_unlock(&list->thread_lock);
						return -EFAULT;
					}
					retval += sizeof(struct hiddev_usage_ref);
				}
			}
			list->tail = (list->tail + 1) & (HIDDEV_BUFFER_SIZE - 1);
		}

	}
	mutex_unlock(&list->thread_lock);

	return retval;
}
