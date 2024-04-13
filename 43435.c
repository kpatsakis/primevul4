int snd_card_file_remove(struct snd_card *card, struct file *file)
{
	struct snd_monitor_file *mfile, *found = NULL;

	spin_lock(&card->files_lock);
	list_for_each_entry(mfile, &card->files_list, list) {
		if (mfile->file == file) {
			list_del(&mfile->list);
			spin_lock(&shutdown_lock);
			list_del(&mfile->shutdown_list);
			spin_unlock(&shutdown_lock);
			if (mfile->disconnected_f_op)
				fops_put(mfile->disconnected_f_op);
			found = mfile;
			break;
		}
	}
	spin_unlock(&card->files_lock);
	if (!found) {
		dev_err(card->dev, "card file remove problem (%p)\n", file);
		return -ENOENT;
	}
	kfree(found);
	put_device(&card->card_dev);
	return 0;
}
