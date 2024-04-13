static int write_note_info(struct elf_note_info *info,
			   struct coredump_params *cprm)
{
	int i;
	struct list_head *t;

	for (i = 0; i < info->numnote; i++)
		if (!writenote(info->notes + i, cprm))
			return 0;

	/* write out the thread status notes section */
	list_for_each(t, &info->thread_list) {
		struct elf_thread_status *tmp =
				list_entry(t, struct elf_thread_status, list);

		for (i = 0; i < tmp->num_notes; i++)
			if (!writenote(&tmp->notes[i], cprm))
				return 0;
	}

	return 1;
}
