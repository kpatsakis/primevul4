static void update_canvas_size(AVCodecContext *avctx, int w, int h)
{
    WebPContext *s = avctx->priv_data;
    if (s->width && s->width != w) {
        av_log(avctx, AV_LOG_WARNING, "Width mismatch. %d != %d\n",
               s->width, w);
    }
    s->width = w;
    if (s->height && s->height != h) {
        av_log(avctx, AV_LOG_WARNING, "Height mismatch. %d != %d\n",
               s->height, h);
    }
    s->height = h;
}
