static int mov_read_tkhd(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    int i, j, e;
    int width;
    int height;
    int display_matrix[3][3];
    int res_display_matrix[3][3] = { { 0 } };
    AVStream *st;
    MOVStreamContext *sc;
    int version;
    int flags;

    if (c->fc->nb_streams < 1)
        return 0;
    st = c->fc->streams[c->fc->nb_streams-1];
    sc = st->priv_data;

    version = avio_r8(pb);
    flags = avio_rb24(pb);
    st->disposition |= (flags & MOV_TKHD_FLAG_ENABLED) ? AV_DISPOSITION_DEFAULT : 0;

    if (version == 1) {
        avio_rb64(pb);
        avio_rb64(pb);
    } else {
        avio_rb32(pb); /* creation time */
        avio_rb32(pb); /* modification time */
    }
    st->id = (int)avio_rb32(pb); /* track id (NOT 0 !)*/
    avio_rb32(pb); /* reserved */

    /* highlevel (considering edits) duration in movie timebase */
    (version == 1) ? avio_rb64(pb) : avio_rb32(pb);
    avio_rb32(pb); /* reserved */
    avio_rb32(pb); /* reserved */

    avio_rb16(pb); /* layer */
    avio_rb16(pb); /* alternate group */
    avio_rb16(pb); /* volume */
    avio_rb16(pb); /* reserved */

    for (i = 0; i < 3; i++) {
        display_matrix[i][0] = avio_rb32(pb);   // 16.16 fixed point
        display_matrix[i][1] = avio_rb32(pb);   // 16.16 fixed point
        display_matrix[i][2] = avio_rb32(pb);   //  2.30 fixed point
    }

    width = avio_rb32(pb);       // 16.16 fixed point track width
    height = avio_rb32(pb);      // 16.16 fixed point track height
    sc->width = width >> 16;
    sc->height = height >> 16;

    for (i = 0; i < 3; i++) {
        const int sh[3] = { 16, 16, 30 };
        for (j = 0; j < 3; j++) {
            for (e = 0; e < 3; e++) {
                res_display_matrix[i][j] +=
                    ((int64_t) display_matrix[i][e] *
                     c->movie_display_matrix[e][j]) >> sh[e];
            }
        }
    }

    if (!IS_MATRIX_IDENT(res_display_matrix)) {
        double rotate;

        av_freep(&sc->display_matrix);
        sc->display_matrix = av_malloc(sizeof(int32_t) * 9);
        if (!sc->display_matrix)
            return AVERROR(ENOMEM);

        for (i = 0; i < 3; i++)
            for (j = 0; j < 3; j++)
                sc->display_matrix[i * 3 + j] = res_display_matrix[i][j];

#if FF_API_OLD_ROTATE_API
        rotate = av_display_rotation_get(sc->display_matrix);
        if (!isnan(rotate)) {
            char rotate_buf[64];
            rotate = -rotate;
            if (rotate < 0) // for backward compatibility
                rotate += 360;
            snprintf(rotate_buf, sizeof(rotate_buf), "%g", rotate);
            av_dict_set(&st->metadata, "rotate", rotate_buf, 0);
        }
#endif
    }

    if (width && height && sc->display_matrix) {
        double disp_transform[2];

        for (i = 0; i < 2; i++)
            disp_transform[i] = hypot(sc->display_matrix[0 + i],
                                      sc->display_matrix[3 + i]);

        if (disp_transform[0] > 0       && disp_transform[1] > 0 &&
            disp_transform[0] < (1<<24) && disp_transform[1] < (1<<24) &&
            fabs((disp_transform[0] / disp_transform[1]) - 1.0) > 0.01)
            st->sample_aspect_ratio = av_d2q(
                disp_transform[0] / disp_transform[1],
                INT_MAX);
    }
    return 0;
}
