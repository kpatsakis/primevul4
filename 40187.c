 int get_dumpable(struct mm_struct *mm)
 {
 	return __get_dumpable(mm->flags);
}
