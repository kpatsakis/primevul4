static void mov_fix_index(MOVContext *mov, AVStream *st)
{
    MOVStreamContext *msc = st->priv_data;
    AVIndexEntry *e_old = st->index_entries;
    int nb_old = st->nb_index_entries;
    const AVIndexEntry *e_old_end = e_old + nb_old;
    const AVIndexEntry *current = NULL;
    MOVStts *ctts_data_old = msc->ctts_data;
    int64_t ctts_index_old = 0;
    int64_t ctts_sample_old = 0;
    int64_t ctts_count_old = msc->ctts_count;
    int64_t edit_list_media_time = 0;
    int64_t edit_list_duration = 0;
    int64_t frame_duration = 0;
    int64_t edit_list_dts_counter = 0;
    int64_t edit_list_dts_entry_end = 0;
    int64_t edit_list_start_ctts_sample = 0;
    int64_t curr_cts;
    int64_t curr_ctts = 0;
    int64_t min_corrected_pts = -1;
    int64_t empty_edits_sum_duration = 0;
    int64_t edit_list_index = 0;
    int64_t index;
    int64_t index_ctts_count;
    int flags;
    int64_t start_dts = 0;
    int64_t edit_list_media_time_dts = 0;
    int64_t edit_list_start_encountered = 0;
    int64_t search_timestamp = 0;
    int64_t* frame_duration_buffer = NULL;
    int num_discarded_begin = 0;
    int first_non_zero_audio_edit = -1;
    int packet_skip_samples = 0;
    MOVIndexRange *current_index_range;
    int i;

    if (!msc->elst_data || msc->elst_count <= 0 || nb_old <= 0) {
        return;
    }

    msc->index_ranges = av_malloc((msc->elst_count + 1) * sizeof(msc->index_ranges[0]));
    if (!msc->index_ranges) {
        av_log(mov->fc, AV_LOG_ERROR, "Cannot allocate index ranges buffer\n");
        return;
    }
    msc->current_index_range = msc->index_ranges;
    current_index_range = msc->index_ranges - 1;

    st->index_entries = NULL;
    st->index_entries_allocated_size = 0;
    st->nb_index_entries = 0;

    msc->ctts_data = NULL;
    msc->ctts_count = 0;
    msc->ctts_index = 0;
    msc->ctts_sample = 0;
    msc->ctts_allocated_size = 0;

    if (msc->dts_shift > 0) {
        edit_list_dts_entry_end -= msc->dts_shift;
        av_log(mov->fc, AV_LOG_DEBUG, "Shifting DTS by %d because of negative CTTS.\n", msc->dts_shift);
    }

    start_dts = edit_list_dts_entry_end;

    while (get_edit_list_entry(mov, msc, edit_list_index, &edit_list_media_time,
                               &edit_list_duration, mov->time_scale)) {
        av_log(mov->fc, AV_LOG_DEBUG, "Processing st: %d, edit list %"PRId64" - media time: %"PRId64", duration: %"PRId64"\n",
               st->index, edit_list_index, edit_list_media_time, edit_list_duration);
        edit_list_index++;
        edit_list_dts_counter = edit_list_dts_entry_end;
        edit_list_dts_entry_end += edit_list_duration;
        num_discarded_begin = 0;
        if (edit_list_media_time == -1) {
            empty_edits_sum_duration += edit_list_duration;
            continue;
        }

        if (st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            if (first_non_zero_audio_edit < 0) {
                first_non_zero_audio_edit = 1;
            } else {
                first_non_zero_audio_edit = 0;
            }

            if (first_non_zero_audio_edit > 0)
                st->skip_samples = msc->start_pad = 0;
        }

        edit_list_media_time_dts = edit_list_media_time;
        if (msc->dts_shift > 0) {
            edit_list_media_time_dts -= msc->dts_shift;
        }

        search_timestamp = edit_list_media_time_dts;
        if (st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            search_timestamp = FFMAX(search_timestamp - msc->time_scale, e_old[0].timestamp);
        }

        index = find_prev_closest_index(st, e_old, nb_old, search_timestamp, 0);
        if (index == -1) {
            av_log(mov->fc, AV_LOG_WARNING,
                   "st: %d edit list: %"PRId64" Missing key frame while searching for timestamp: %"PRId64"\n",
                   st->index, edit_list_index, search_timestamp);
            index = find_prev_closest_index(st, e_old, nb_old, search_timestamp, AVSEEK_FLAG_ANY);

            if (index == -1) {
                av_log(mov->fc, AV_LOG_WARNING,
                       "st: %d edit list %"PRId64" Cannot find an index entry before timestamp: %"PRId64".\n"
                       "Rounding edit list media time to zero.\n",
                       st->index, edit_list_index, search_timestamp);
                index = 0;
                edit_list_media_time = 0;
            }
        }
        current = e_old + index;

        ctts_index_old = 0;
        ctts_sample_old = 0;

        for (index_ctts_count = 0; index_ctts_count < index; index_ctts_count++) {
            if (ctts_data_old && ctts_index_old < ctts_count_old) {
                ctts_sample_old++;
                if (ctts_data_old[ctts_index_old].count == ctts_sample_old) {
                    ctts_index_old++;
                    ctts_sample_old = 0;
                }
            }
        }

        edit_list_start_ctts_sample = ctts_sample_old;

        edit_list_start_encountered = 0;
        for (; current < e_old_end; current++, index++) {
            frame_duration = (current + 1 <  e_old_end) ?
                             ((current + 1)->timestamp - current->timestamp) : edit_list_duration;

            flags = current->flags;

            curr_cts = current->timestamp + msc->dts_shift;
            curr_ctts = 0;

            if (ctts_data_old && ctts_index_old < ctts_count_old) {
                curr_ctts = ctts_data_old[ctts_index_old].duration;
                av_log(mov->fc, AV_LOG_DEBUG, "stts: %"PRId64" ctts: %"PRId64", ctts_index: %"PRId64", ctts_count: %"PRId64"\n",
                       curr_cts, curr_ctts, ctts_index_old, ctts_count_old);
                curr_cts += curr_ctts;
                ctts_sample_old++;
                if (ctts_sample_old == ctts_data_old[ctts_index_old].count) {
                    if (add_ctts_entry(&msc->ctts_data, &msc->ctts_count,
                                       &msc->ctts_allocated_size,
                                       ctts_data_old[ctts_index_old].count - edit_list_start_ctts_sample,
                                       ctts_data_old[ctts_index_old].duration) == -1) {
                        av_log(mov->fc, AV_LOG_ERROR, "Cannot add CTTS entry %"PRId64" - {%"PRId64", %d}\n",
                               ctts_index_old,
                               ctts_data_old[ctts_index_old].count - edit_list_start_ctts_sample,
                               ctts_data_old[ctts_index_old].duration);
                        break;
                    }
                    ctts_index_old++;
                    ctts_sample_old = 0;
                    edit_list_start_ctts_sample = 0;
                }
            }

            if (curr_cts < edit_list_media_time || curr_cts >= (edit_list_duration + edit_list_media_time)) {
                if (st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO && st->codecpar->codec_id != AV_CODEC_ID_VORBIS &&
                    curr_cts < edit_list_media_time && curr_cts + frame_duration > edit_list_media_time &&
                    first_non_zero_audio_edit > 0) {
                    packet_skip_samples = edit_list_media_time - curr_cts;
                    st->skip_samples += packet_skip_samples;

                    edit_list_dts_counter -= packet_skip_samples;
                    if (edit_list_start_encountered == 0)  {
                        edit_list_start_encountered = 1;
                        if (frame_duration_buffer) {
                            fix_index_entry_timestamps(st, st->nb_index_entries, edit_list_dts_counter,
                                                       frame_duration_buffer, num_discarded_begin);
                            av_freep(&frame_duration_buffer);
                        }
                    }

                    av_log(mov->fc, AV_LOG_DEBUG, "skip %d audio samples from curr_cts: %"PRId64"\n", packet_skip_samples, curr_cts);
                } else {
                    flags |= AVINDEX_DISCARD_FRAME;
                    av_log(mov->fc, AV_LOG_DEBUG, "drop a frame at curr_cts: %"PRId64" @ %"PRId64"\n", curr_cts, index);

                    if (st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO && edit_list_start_encountered == 0) {
                        num_discarded_begin++;
                        frame_duration_buffer = av_realloc(frame_duration_buffer,
                                                           num_discarded_begin * sizeof(int64_t));
                        if (!frame_duration_buffer) {
                            av_log(mov->fc, AV_LOG_ERROR, "Cannot reallocate frame duration buffer\n");
                            break;
                        }
                        frame_duration_buffer[num_discarded_begin - 1] = frame_duration;

                        if (first_non_zero_audio_edit > 0 && st->codecpar->codec_id != AV_CODEC_ID_VORBIS) {
                            st->skip_samples += frame_duration;
                            msc->start_pad = st->skip_samples;
                        }
                    }
                }
            } else {
                if (min_corrected_pts < 0) {
                    min_corrected_pts = edit_list_dts_counter + curr_ctts + msc->dts_shift;
                } else {
                    min_corrected_pts = FFMIN(min_corrected_pts, edit_list_dts_counter + curr_ctts + msc->dts_shift);
                }
                if (edit_list_start_encountered == 0) {
                    edit_list_start_encountered = 1;
                    if (st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO && frame_duration_buffer) {
                        fix_index_entry_timestamps(st, st->nb_index_entries, edit_list_dts_counter,
                                                   frame_duration_buffer, num_discarded_begin);
                        av_freep(&frame_duration_buffer);
                    }
                }
            }

            if (add_index_entry(st, current->pos, edit_list_dts_counter, current->size,
                                current->min_distance, flags) == -1) {
                av_log(mov->fc, AV_LOG_ERROR, "Cannot add index entry\n");
                break;
            }

            if (current_index_range < msc->index_ranges || index != current_index_range->end) {
                current_index_range++;
                current_index_range->start = index;
            }
            current_index_range->end = index + 1;

            if (edit_list_start_encountered > 0) {
                edit_list_dts_counter = edit_list_dts_counter + frame_duration;
            }

            if (((curr_cts + frame_duration) >= (edit_list_duration + edit_list_media_time)) &&
                ((flags & AVINDEX_KEYFRAME) || ((st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)))) {

                if (ctts_data_old && ctts_sample_old != 0) {
                    if (add_ctts_entry(&msc->ctts_data, &msc->ctts_count,
                                       &msc->ctts_allocated_size,
                                       ctts_sample_old - edit_list_start_ctts_sample,
                                       ctts_data_old[ctts_index_old].duration) == -1) {
                        av_log(mov->fc, AV_LOG_ERROR, "Cannot add CTTS entry %"PRId64" - {%"PRId64", %d}\n",
                               ctts_index_old, ctts_sample_old - edit_list_start_ctts_sample,
                               ctts_data_old[ctts_index_old].duration);
                        break;
                    }
                }
                break;
            }
        }
    }
    min_corrected_pts -= empty_edits_sum_duration;

    if (st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO && min_corrected_pts > 0) {
        av_log(mov->fc, AV_LOG_DEBUG, "Offset DTS by %"PRId64" to make first pts zero.\n", min_corrected_pts);
        for (i = 0; i < st->nb_index_entries; ++i) {
            st->index_entries[i].timestamp -= min_corrected_pts;
        }
    }

    st->duration = edit_list_dts_entry_end - start_dts;

    av_free(e_old);
    av_free(ctts_data_old);

    current_index_range++;
    current_index_range->start = 0;
    current_index_range->end = 0;
    msc->current_index = msc->index_ranges[0].start;
}
