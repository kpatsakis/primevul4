static int apply_color_transform(WebPContext *s)
{
    ImageContext *img, *cimg;
    int x, y, cx, cy;
    uint8_t *p, *cp;

    img  = &s->image[IMAGE_ROLE_ARGB];
    cimg = &s->image[IMAGE_ROLE_COLOR_TRANSFORM];

    for (y = 0; y < img->frame->height; y++) {
        for (x = 0; x < img->frame->width; x++) {
            cx = x >> cimg->size_reduction;
            cy = y >> cimg->size_reduction;
            cp = GET_PIXEL(cimg->frame, cx, cy);
            p  = GET_PIXEL(img->frame,   x,  y);

            p[1] += color_transform_delta(cp[3], p[2]);
            p[3] += color_transform_delta(cp[2], p[2]) +
                    color_transform_delta(cp[1], p[1]);
        }
    }
    return 0;
}
