static void copy_chroma(AVFrame *dst, AVFrame *src, int width, int height)
{
    int i, j;

    for (j = 1; j < 3; j++) {
        for (i = 0; i < height / 2; i++)
            memcpy(dst->data[j] + i * dst->linesize[j],
                   src->data[j] + i * src->linesize[j], width / 2);
    }
}
