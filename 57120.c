static void free_buffers(VP8Context *s)
{
    int i;
    if (s->thread_data)
        for (i = 0; i < MAX_THREADS; i++) {
#if HAVE_THREADS
            pthread_cond_destroy(&s->thread_data[i].cond);
            pthread_mutex_destroy(&s->thread_data[i].lock);
#endif
            av_freep(&s->thread_data[i].filter_strength);
        }
    av_freep(&s->thread_data);
    av_freep(&s->macroblocks_base);
    av_freep(&s->intra4x4_pred_mode_top);
    av_freep(&s->top_nnz);
    av_freep(&s->top_border);

    s->macroblocks = NULL;
}
