static int apply_subtract_green_transform(WebPContext *s)
{
    int x, y;
    ImageContext *img = &s->image[IMAGE_ROLE_ARGB];

    for (y = 0; y < img->frame->height; y++) {
        for (x = 0; x < img->frame->width; x++) {
            uint8_t *p = GET_PIXEL(img->frame, x, y);
            p[1] += p[2];
            p[3] += p[2];
        }
    }
    return 0;
}
