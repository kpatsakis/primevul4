static int parse_transform_color(WebPContext *s)
{
    int block_bits, blocks_w, blocks_h, ret;

    PARSE_BLOCK_SIZE(s->width, s->height);

    ret = decode_entropy_coded_image(s, IMAGE_ROLE_COLOR_TRANSFORM, blocks_w,
                                     blocks_h);
    if (ret < 0)
        return ret;

    s->image[IMAGE_ROLE_COLOR_TRANSFORM].size_reduction = block_bits;

    return 0;
}
