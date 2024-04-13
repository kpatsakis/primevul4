static int decode_header_trees(SmackVContext *smk) {
    BitstreamContext bc;
    int mmap_size, mclr_size, full_size, type_size, ret;

    mmap_size = AV_RL32(smk->avctx->extradata);
    mclr_size = AV_RL32(smk->avctx->extradata + 4);
    full_size = AV_RL32(smk->avctx->extradata + 8);
    type_size = AV_RL32(smk->avctx->extradata + 12);

    bitstream_init8(&bc, smk->avctx->extradata + 16, smk->avctx->extradata_size - 16);

    if (!bitstream_read_bit(&bc)) {
        av_log(smk->avctx, AV_LOG_INFO, "Skipping MMAP tree\n");
        smk->mmap_tbl = av_malloc(sizeof(int) * 2);
        if (!smk->mmap_tbl)
            return AVERROR(ENOMEM);
        smk->mmap_tbl[0] = 0;
        smk->mmap_last[0] = smk->mmap_last[1] = smk->mmap_last[2] = 1;
    } else {
        if ((ret = smacker_decode_header_tree(smk, &bc, &smk->mmap_tbl, smk->mmap_last, mmap_size)) < 0)
            return ret;
    }
    if (!bitstream_read_bit(&bc)) {
        av_log(smk->avctx, AV_LOG_INFO, "Skipping MCLR tree\n");
        smk->mclr_tbl = av_malloc(sizeof(int) * 2);
        if (!smk->mclr_tbl)
            return AVERROR(ENOMEM);
        smk->mclr_tbl[0] = 0;
        smk->mclr_last[0] = smk->mclr_last[1] = smk->mclr_last[2] = 1;
    } else {
        if ((ret = smacker_decode_header_tree(smk, &bc, &smk->mclr_tbl, smk->mclr_last, mclr_size)) < 0)
            return ret;
    }
    if (!bitstream_read_bit(&bc)) {
        av_log(smk->avctx, AV_LOG_INFO, "Skipping FULL tree\n");
        smk->full_tbl = av_malloc(sizeof(int) * 2);
        if (!smk->full_tbl)
            return AVERROR(ENOMEM);
        smk->full_tbl[0] = 0;
        smk->full_last[0] = smk->full_last[1] = smk->full_last[2] = 1;
    } else {
        if ((ret = smacker_decode_header_tree(smk, &bc, &smk->full_tbl, smk->full_last, full_size)) < 0)
            return ret;
    }
    if (!bitstream_read_bit(&bc)) {
        av_log(smk->avctx, AV_LOG_INFO, "Skipping TYPE tree\n");
        smk->type_tbl = av_malloc(sizeof(int) * 2);
        if (!smk->type_tbl)
            return AVERROR(ENOMEM);
        smk->type_tbl[0] = 0;
        smk->type_last[0] = smk->type_last[1] = smk->type_last[2] = 1;
    } else {
        if ((ret = smacker_decode_header_tree(smk, &bc, &smk->type_tbl, smk->type_last, type_size)) < 0)
            return ret;
    }

    return 0;
}
