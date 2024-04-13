void set_dumpable(struct mm_struct *mm, int value)
{
	switch (value) {
	case SUID_DUMP_DISABLE:
		clear_bit(MMF_DUMPABLE, &mm->flags);
		smp_wmb();
		clear_bit(MMF_DUMP_SECURELY, &mm->flags);
		break;
	case SUID_DUMP_USER:
		set_bit(MMF_DUMPABLE, &mm->flags);
		smp_wmb();
		clear_bit(MMF_DUMP_SECURELY, &mm->flags);
		break;
	case SUID_DUMP_ROOT:
		set_bit(MMF_DUMP_SECURELY, &mm->flags);
		smp_wmb();
		set_bit(MMF_DUMPABLE, &mm->flags);
		break;
	}
}
