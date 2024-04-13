void clamp_mv(VP8mvbounds *s, VP56mv *dst, const VP56mv *src)
{
    dst->x = av_clip(src->x, av_clip(s->mv_min.x, INT16_MIN, INT16_MAX),
                             av_clip(s->mv_max.x, INT16_MIN, INT16_MAX));
    dst->y = av_clip(src->y, av_clip(s->mv_min.y, INT16_MIN, INT16_MAX),
                             av_clip(s->mv_max.y, INT16_MIN, INT16_MAX));
}
