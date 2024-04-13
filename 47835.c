static int elf_note_info_init(struct elf_note_info *info)
{
	memset(info, 0, sizeof(*info));
	INIT_LIST_HEAD(&info->thread_list);

	/* Allocate space for ELF notes */
	info->notes = kmalloc(8 * sizeof(struct memelfnote), GFP_KERNEL);
	if (!info->notes)
		return 0;
	info->psinfo = kmalloc(sizeof(*info->psinfo), GFP_KERNEL);
	if (!info->psinfo)
		return 0;
	info->prstatus = kmalloc(sizeof(*info->prstatus), GFP_KERNEL);
	if (!info->prstatus)
		return 0;
	info->fpu = kmalloc(sizeof(*info->fpu), GFP_KERNEL);
	if (!info->fpu)
		return 0;
#ifdef ELF_CORE_COPY_XFPREGS
	info->xfpu = kmalloc(sizeof(*info->xfpu), GFP_KERNEL);
	if (!info->xfpu)
		return 0;
#endif
	return 1;
}
