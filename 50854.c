static ssize_t proc_pid_cmdline_read(struct file *file, char __user *buf,
				     size_t _count, loff_t *pos)
{
	struct task_struct *tsk;
	struct mm_struct *mm;
	char *page;
	unsigned long count = _count;
	unsigned long arg_start, arg_end, env_start, env_end;
	unsigned long len1, len2, len;
	unsigned long p;
	char c;
	ssize_t rv;

	BUG_ON(*pos < 0);

	tsk = get_proc_task(file_inode(file));
	if (!tsk)
		return -ESRCH;
	mm = get_task_mm(tsk);
	put_task_struct(tsk);
	if (!mm)
		return 0;
	/* Check if process spawned far enough to have cmdline. */
	if (!mm->env_end) {
		rv = 0;
		goto out_mmput;
	}

	page = (char *)__get_free_page(GFP_TEMPORARY);
	if (!page) {
		rv = -ENOMEM;
		goto out_mmput;
	}

	down_read(&mm->mmap_sem);
	arg_start = mm->arg_start;
	arg_end = mm->arg_end;
	env_start = mm->env_start;
	env_end = mm->env_end;
	up_read(&mm->mmap_sem);

	BUG_ON(arg_start > arg_end);
	BUG_ON(env_start > env_end);

	len1 = arg_end - arg_start;
	len2 = env_end - env_start;

	/* Empty ARGV. */
	if (len1 == 0) {
		rv = 0;
		goto out_free_page;
	}
	/*
	 * Inherently racy -- command line shares address space
	 * with code and data.
	 */
	rv = access_remote_vm(mm, arg_end - 1, &c, 1, 0);
	if (rv <= 0)
		goto out_free_page;

	rv = 0;

	if (c == '\0') {
		/* Command line (set of strings) occupies whole ARGV. */
		if (len1 <= *pos)
			goto out_free_page;

		p = arg_start + *pos;
		len = len1 - *pos;
		while (count > 0 && len > 0) {
			unsigned int _count;
			int nr_read;

			_count = min3(count, len, PAGE_SIZE);
			nr_read = access_remote_vm(mm, p, page, _count, 0);
			if (nr_read < 0)
				rv = nr_read;
			if (nr_read <= 0)
				goto out_free_page;

			if (copy_to_user(buf, page, nr_read)) {
				rv = -EFAULT;
				goto out_free_page;
			}

			p	+= nr_read;
			len	-= nr_read;
			buf	+= nr_read;
			count	-= nr_read;
			rv	+= nr_read;
		}
	} else {
		/*
		 * Command line (1 string) occupies ARGV and maybe
		 * extends into ENVP.
		 */
		if (len1 + len2 <= *pos)
			goto skip_argv_envp;
		if (len1 <= *pos)
			goto skip_argv;

		p = arg_start + *pos;
		len = len1 - *pos;
		while (count > 0 && len > 0) {
			unsigned int _count, l;
			int nr_read;
			bool final;

			_count = min3(count, len, PAGE_SIZE);
			nr_read = access_remote_vm(mm, p, page, _count, 0);
			if (nr_read < 0)
				rv = nr_read;
			if (nr_read <= 0)
				goto out_free_page;

			/*
			 * Command line can be shorter than whole ARGV
			 * even if last "marker" byte says it is not.
			 */
			final = false;
			l = strnlen(page, nr_read);
			if (l < nr_read) {
				nr_read = l;
				final = true;
			}

			if (copy_to_user(buf, page, nr_read)) {
				rv = -EFAULT;
				goto out_free_page;
			}

			p	+= nr_read;
			len	-= nr_read;
			buf	+= nr_read;
			count	-= nr_read;
			rv	+= nr_read;

			if (final)
				goto out_free_page;
		}
skip_argv:
		/*
		 * Command line (1 string) occupies ARGV and
		 * extends into ENVP.
		 */
		if (len1 <= *pos) {
			p = env_start + *pos - len1;
			len = len1 + len2 - *pos;
		} else {
			p = env_start;
			len = len2;
		}
		while (count > 0 && len > 0) {
			unsigned int _count, l;
			int nr_read;
			bool final;

			_count = min3(count, len, PAGE_SIZE);
			nr_read = access_remote_vm(mm, p, page, _count, 0);
			if (nr_read < 0)
				rv = nr_read;
			if (nr_read <= 0)
				goto out_free_page;

			/* Find EOS. */
			final = false;
			l = strnlen(page, nr_read);
			if (l < nr_read) {
				nr_read = l;
				final = true;
			}

			if (copy_to_user(buf, page, nr_read)) {
				rv = -EFAULT;
				goto out_free_page;
			}

			p	+= nr_read;
			len	-= nr_read;
			buf	+= nr_read;
			count	-= nr_read;
			rv	+= nr_read;

			if (final)
				goto out_free_page;
		}
skip_argv_envp:
		;
	}

out_free_page:
	free_page((unsigned long)page);
out_mmput:
	mmput(mm);
	if (rv > 0)
		*pos += rv;
	return rv;
}
