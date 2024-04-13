static int mxf_read_generic_descriptor(void *arg, AVIOContext *pb, int tag, int size, UID uid, int64_t klv_offset)
{
    MXFDescriptor *descriptor = arg;
    int entry_count, entry_size;

    switch(tag) {
    case 0x3F01:
        return mxf_read_strong_ref_array(pb, &descriptor->sub_descriptors_refs,
                                             &descriptor->sub_descriptors_count);
    case 0x3002: /* ContainerDuration */
        descriptor->duration = avio_rb64(pb);
        break;
    case 0x3004:
        avio_read(pb, descriptor->essence_container_ul, 16);
        break;
    case 0x3005:
        avio_read(pb, descriptor->codec_ul, 16);
        break;
    case 0x3006:
        descriptor->linked_track_id = avio_rb32(pb);
        break;
    case 0x3201: /* PictureEssenceCoding */
        avio_read(pb, descriptor->essence_codec_ul, 16);
        break;
    case 0x3203:
        descriptor->width = avio_rb32(pb);
        break;
    case 0x3202:
        descriptor->height = avio_rb32(pb);
        break;
    case 0x320C:
        descriptor->frame_layout = avio_r8(pb);
        break;
    case 0x320D:
        entry_count = avio_rb32(pb);
        entry_size = avio_rb32(pb);
        if (entry_size == 4) {
            if (entry_count > 0)
                descriptor->video_line_map[0] = avio_rb32(pb);
            else
                descriptor->video_line_map[0] = 0;
            if (entry_count > 1)
                descriptor->video_line_map[1] = avio_rb32(pb);
            else
                descriptor->video_line_map[1] = 0;
        } else
            av_log(NULL, AV_LOG_WARNING, "VideoLineMap element size %d currently not supported\n", entry_size);
        break;
    case 0x320E:
        descriptor->aspect_ratio.num = avio_rb32(pb);
        descriptor->aspect_ratio.den = avio_rb32(pb);
        break;
    case 0x3212:
        descriptor->field_dominance = avio_r8(pb);
        break;
    case 0x3301:
        descriptor->component_depth = avio_rb32(pb);
        break;
    case 0x3302:
        descriptor->horiz_subsampling = avio_rb32(pb);
        break;
    case 0x3308:
        descriptor->vert_subsampling = avio_rb32(pb);
        break;
    case 0x3D03:
        descriptor->sample_rate.num = avio_rb32(pb);
        descriptor->sample_rate.den = avio_rb32(pb);
        break;
    case 0x3D06: /* SoundEssenceCompression */
        avio_read(pb, descriptor->essence_codec_ul, 16);
        break;
    case 0x3D07:
        descriptor->channels = avio_rb32(pb);
        break;
    case 0x3D01:
        descriptor->bits_per_sample = avio_rb32(pb);
        break;
    case 0x3401:
        mxf_read_pixel_layout(pb, descriptor);
        break;
    default:
        /* Private uid used by SONY C0023S01.mxf */
        if (IS_KLV_KEY(uid, mxf_sony_mpeg4_extradata)) {
            if (descriptor->extradata)
                av_log(NULL, AV_LOG_WARNING, "Duplicate sony_mpeg4_extradata\n");
            av_free(descriptor->extradata);
            descriptor->extradata_size = 0;
            descriptor->extradata = av_malloc(size);
            if (!descriptor->extradata)
                return AVERROR(ENOMEM);
            descriptor->extradata_size = size;
            avio_read(pb, descriptor->extradata, size);
        }
        if (IS_KLV_KEY(uid, mxf_jp2k_rsiz)) {
            uint32_t rsiz = avio_rb16(pb);
            if (rsiz == FF_PROFILE_JPEG2000_DCINEMA_2K ||
                rsiz == FF_PROFILE_JPEG2000_DCINEMA_4K)
                descriptor->pix_fmt = AV_PIX_FMT_XYZ12;
        }
        break;
    }
    return 0;
}
