void perf_output_sample(struct perf_output_handle *handle,
			struct perf_event_header *header,
			struct perf_sample_data *data,
			struct perf_event *event)
{
	u64 sample_type = data->type;

	perf_output_put(handle, *header);

	if (sample_type & PERF_SAMPLE_IDENTIFIER)
		perf_output_put(handle, data->id);

	if (sample_type & PERF_SAMPLE_IP)
		perf_output_put(handle, data->ip);

	if (sample_type & PERF_SAMPLE_TID)
		perf_output_put(handle, data->tid_entry);

	if (sample_type & PERF_SAMPLE_TIME)
		perf_output_put(handle, data->time);

	if (sample_type & PERF_SAMPLE_ADDR)
		perf_output_put(handle, data->addr);

	if (sample_type & PERF_SAMPLE_ID)
		perf_output_put(handle, data->id);

	if (sample_type & PERF_SAMPLE_STREAM_ID)
		perf_output_put(handle, data->stream_id);

	if (sample_type & PERF_SAMPLE_CPU)
		perf_output_put(handle, data->cpu_entry);

	if (sample_type & PERF_SAMPLE_PERIOD)
		perf_output_put(handle, data->period);

	if (sample_type & PERF_SAMPLE_READ)
		perf_output_read(handle, event);

	if (sample_type & PERF_SAMPLE_CALLCHAIN) {
		if (data->callchain) {
			int size = 1;

			if (data->callchain)
				size += data->callchain->nr;

			size *= sizeof(u64);

			__output_copy(handle, data->callchain, size);
		} else {
			u64 nr = 0;
			perf_output_put(handle, nr);
		}
	}

	if (sample_type & PERF_SAMPLE_RAW) {
		struct perf_raw_record *raw = data->raw;

		if (raw) {
			struct perf_raw_frag *frag = &raw->frag;

			perf_output_put(handle, raw->size);
			do {
				if (frag->copy) {
					__output_custom(handle, frag->copy,
							frag->data, frag->size);
				} else {
					__output_copy(handle, frag->data,
						      frag->size);
				}
				if (perf_raw_frag_last(frag))
					break;
				frag = frag->next;
			} while (1);
			if (frag->pad)
				__output_skip(handle, NULL, frag->pad);
		} else {
			struct {
				u32	size;
				u32	data;
			} raw = {
				.size = sizeof(u32),
				.data = 0,
			};
			perf_output_put(handle, raw);
		}
	}

	if (sample_type & PERF_SAMPLE_BRANCH_STACK) {
		if (data->br_stack) {
			size_t size;

			size = data->br_stack->nr
			     * sizeof(struct perf_branch_entry);

			perf_output_put(handle, data->br_stack->nr);
			perf_output_copy(handle, data->br_stack->entries, size);
		} else {
			/*
			 * we always store at least the value of nr
			 */
			u64 nr = 0;
			perf_output_put(handle, nr);
		}
	}

	if (sample_type & PERF_SAMPLE_REGS_USER) {
		u64 abi = data->regs_user.abi;

		/*
		 * If there are no regs to dump, notice it through
		 * first u64 being zero (PERF_SAMPLE_REGS_ABI_NONE).
		 */
		perf_output_put(handle, abi);

		if (abi) {
			u64 mask = event->attr.sample_regs_user;
			perf_output_sample_regs(handle,
						data->regs_user.regs,
						mask);
		}
	}

	if (sample_type & PERF_SAMPLE_STACK_USER) {
		perf_output_sample_ustack(handle,
					  data->stack_user_size,
					  data->regs_user.regs);
	}

	if (sample_type & PERF_SAMPLE_WEIGHT)
		perf_output_put(handle, data->weight);

	if (sample_type & PERF_SAMPLE_DATA_SRC)
		perf_output_put(handle, data->data_src.val);

	if (sample_type & PERF_SAMPLE_TRANSACTION)
		perf_output_put(handle, data->txn);

	if (sample_type & PERF_SAMPLE_REGS_INTR) {
		u64 abi = data->regs_intr.abi;
		/*
		 * If there are no regs to dump, notice it through
		 * first u64 being zero (PERF_SAMPLE_REGS_ABI_NONE).
		 */
		perf_output_put(handle, abi);

		if (abi) {
			u64 mask = event->attr.sample_regs_intr;

			perf_output_sample_regs(handle,
						data->regs_intr.regs,
						mask);
		}
	}

	if (!event->attr.watermark) {
		int wakeup_events = event->attr.wakeup_events;

		if (wakeup_events) {
			struct ring_buffer *rb = handle->rb;
			int events = local_inc_return(&rb->events);

			if (events >= wakeup_events) {
				local_sub(wakeup_events, &rb->events);
				local_inc(&rb->wakeup);
			}
		}
	}
}
