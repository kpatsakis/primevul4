static int snd_disconnect_release(struct inode *inode, struct file *file)
{
	struct snd_monitor_file *df = NULL, *_df;

	spin_lock(&shutdown_lock);
	list_for_each_entry(_df, &shutdown_files, shutdown_list) {
		if (_df->file == file) {
			df = _df;
			list_del_init(&df->shutdown_list);
			break;
		}
	}
	spin_unlock(&shutdown_lock);

	if (likely(df)) {
		if ((file->f_flags & FASYNC) && df->disconnected_f_op->fasync)
			df->disconnected_f_op->fasync(-1, file, 0);
		return df->disconnected_f_op->release(inode, file);
	}

	panic("%s(%p, %p) failed!", __func__, inode, file);
}
