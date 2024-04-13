static int mxf_read_essence_group(void *arg, AVIOContext *pb, int tag, int size, UID uid, int64_t klv_offset)
{
    MXFEssenceGroup *essence_group = arg;
    switch (tag) {
    case 0x0202:
        essence_group->duration = avio_rb64(pb);
        break;
    case 0x0501:
        return mxf_read_strong_ref_array(pb, &essence_group->structural_components_refs,
                                             &essence_group->structural_components_count);
    }
    return 0;
}
