void perf_prepare_sample(struct perf_event_header *header,
			 struct perf_sample_data *data,
			 struct perf_event *event,
			 struct pt_regs *regs)
{
	u64 sample_type = event->attr.sample_type;

	header->type = PERF_RECORD_SAMPLE;
	header->size = sizeof(*header) + event->header_size;

	header->misc = 0;
	header->misc |= perf_misc_flags(regs);

	__perf_event_header__init_id(header, data, event);

	if (sample_type & PERF_SAMPLE_IP)
		data->ip = perf_instruction_pointer(regs);

	if (sample_type & PERF_SAMPLE_CALLCHAIN) {
		int size = 1;

		data->callchain = perf_callchain(event, regs);

		if (data->callchain)
			size += data->callchain->nr;

		header->size += size * sizeof(u64);
	}

	if (sample_type & PERF_SAMPLE_RAW) {
		int size = sizeof(u32);

		if (data->raw)
			size += data->raw->size;
		else
			size += sizeof(u32);

		WARN_ON_ONCE(size & (sizeof(u64)-1));
		header->size += size;
	}

	if (sample_type & PERF_SAMPLE_BRANCH_STACK) {
		int size = sizeof(u64); /* nr */
		if (data->br_stack) {
			size += data->br_stack->nr
			      * sizeof(struct perf_branch_entry);
		}
		header->size += size;
	}

	if (sample_type & PERF_SAMPLE_REGS_USER) {
		/* regs dump ABI info */
		int size = sizeof(u64);

		perf_sample_regs_user(&data->regs_user, regs);

		if (data->regs_user.regs) {
			u64 mask = event->attr.sample_regs_user;
			size += hweight64(mask) * sizeof(u64);
		}

		header->size += size;
	}

	if (sample_type & PERF_SAMPLE_STACK_USER) {
		/*
		 * Either we need PERF_SAMPLE_STACK_USER bit to be allways
		 * processed as the last one or have additional check added
		 * in case new sample type is added, because we could eat
		 * up the rest of the sample size.
		 */
		struct perf_regs_user *uregs = &data->regs_user;
		u16 stack_size = event->attr.sample_stack_user;
		u16 size = sizeof(u64);

		if (!uregs->abi)
			perf_sample_regs_user(uregs, regs);

		stack_size = perf_sample_ustack_size(stack_size, header->size,
						     uregs->regs);

		/*
		 * If there is something to dump, add space for the dump
		 * itself and for the field that tells the dynamic size,
		 * which is how many have been actually dumped.
		 */
		if (stack_size)
			size += sizeof(u64) + stack_size;

		data->stack_user_size = stack_size;
		header->size += size;
	}
}
