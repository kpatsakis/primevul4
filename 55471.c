static int mov_parse_uuid_spherical(MOVStreamContext *sc, AVIOContext *pb, size_t len)
{
    int ret = 0;
    uint8_t *buffer = av_malloc(len + 1);
    const char *val;

    if (!buffer)
        return AVERROR(ENOMEM);
    buffer[len] = '\0';

    ret = ffio_read_size(pb, buffer, len);
    if (ret < 0)
        goto out;

    /* Check for mandatory keys and values, try to support XML as best-effort */
    if (av_stristr(buffer, "<GSpherical:StitchingSoftware>") &&
        (val = av_stristr(buffer, "<GSpherical:Spherical>")) &&
        av_stristr(val, "true") &&
        (val = av_stristr(buffer, "<GSpherical:Stitched>")) &&
        av_stristr(val, "true") &&
        (val = av_stristr(buffer, "<GSpherical:ProjectionType>")) &&
        av_stristr(val, "equirectangular")) {
        sc->spherical = av_spherical_alloc(&sc->spherical_size);
        if (!sc->spherical)
            goto out;

        sc->spherical->projection = AV_SPHERICAL_EQUIRECTANGULAR;

        if (av_stristr(buffer, "<GSpherical:StereoMode>")) {
            enum AVStereo3DType mode;

            if (av_stristr(buffer, "left-right"))
                mode = AV_STEREO3D_SIDEBYSIDE;
            else if (av_stristr(buffer, "top-bottom"))
                mode = AV_STEREO3D_TOPBOTTOM;
            else
                mode = AV_STEREO3D_2D;

            sc->stereo3d = av_stereo3d_alloc();
            if (!sc->stereo3d)
                goto out;

            sc->stereo3d->type = mode;
        }

        /* orientation */
        val = av_stristr(buffer, "<GSpherical:InitialViewHeadingDegrees>");
        if (val)
            sc->spherical->yaw = strtol(val, NULL, 10) * (1 << 16);
        val = av_stristr(buffer, "<GSpherical:InitialViewPitchDegrees>");
        if (val)
            sc->spherical->pitch = strtol(val, NULL, 10) * (1 << 16);
        val = av_stristr(buffer, "<GSpherical:InitialViewRollDegrees>");
        if (val)
            sc->spherical->roll = strtol(val, NULL, 10) * (1 << 16);
    }

out:
    av_free(buffer);
    return ret;
}
