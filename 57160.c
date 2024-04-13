static VP8Frame *vp8_find_free_buffer(VP8Context *s)
{
    VP8Frame *frame = NULL;
    int i;

    for (i = 0; i < 5; i++)
        if (&s->frames[i] != s->framep[VP56_FRAME_CURRENT]  &&
            &s->frames[i] != s->framep[VP56_FRAME_PREVIOUS] &&
            &s->frames[i] != s->framep[VP56_FRAME_GOLDEN]   &&
            &s->frames[i] != s->framep[VP56_FRAME_GOLDEN2]) {
            frame = &s->frames[i];
            break;
        }
    if (i == 5) {
        av_log(s->avctx, AV_LOG_FATAL, "Ran out of free frames!\n");
        abort();
    }
    if (frame->tf.f->data[0])
        vp8_release_frame(s, frame);

    return frame;
}
