static int fault_in_user_writeable(u32 __user *uaddr)
{
	struct mm_struct *mm = current->mm;
	int ret;

	down_read(&mm->mmap_sem);
	ret = fixup_user_fault(current, mm, (unsigned long)uaddr,
			       FAULT_FLAG_WRITE);
	up_read(&mm->mmap_sem);

	return ret < 0 ? ret : 0;
}
