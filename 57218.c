static inline int get_duration(AVIStream *ast, int len)
{
    if (ast->sample_size)
        return len;
    else if (ast->dshow_block_align)
        return (len + ast->dshow_block_align - 1) / ast->dshow_block_align;
    else
        return 1;
}
