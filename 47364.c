void ff_h264_flush_change(H264Context *h)
{
    int i, j;

    h->outputed_poc          = h->next_outputed_poc = INT_MIN;
    h->prev_interlaced_frame = 1;
    idr(h);

    h->prev_frame_num = -1;
    if (h->cur_pic_ptr) {
        h->cur_pic_ptr->reference = 0;
        for (j=i=0; h->delayed_pic[i]; i++)
            if (h->delayed_pic[i] != h->cur_pic_ptr)
                h->delayed_pic[j++] = h->delayed_pic[i];
        h->delayed_pic[j] = NULL;
    }
    h->first_field = 0;
    memset(h->ref_list[0], 0, sizeof(h->ref_list[0]));
    memset(h->ref_list[1], 0, sizeof(h->ref_list[1]));
    memset(h->default_ref_list[0], 0, sizeof(h->default_ref_list[0]));
    memset(h->default_ref_list[1], 0, sizeof(h->default_ref_list[1]));
    ff_h264_reset_sei(h);
    h->recovery_frame = -1;
    h->frame_recovered = 0;
    h->list_count = 0;
    h->current_slice = 0;
    h->mmco_reset = 1;
}
