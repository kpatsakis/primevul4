void generate_inter_prediction_samples(base_context* ctx,
                                       const slice_segment_header* shdr,
                                       de265_image* img,
                                       int xC,int yC,
                                       int xB,int yB,
                                       int nCS, int nPbW,int nPbH,
                                       const PBMotion* vi)
{
  int xP = xC+xB;
  int yP = yC+yB;

  void*  pixels[3];
  int    stride[3];

  const pic_parameter_set* pps = shdr->pps.get();
  const seq_parameter_set* sps = pps->sps.get();

  const int SubWidthC  = sps->SubWidthC;
  const int SubHeightC = sps->SubHeightC;

  pixels[0] = img->get_image_plane_at_pos_any_depth(0,xP,yP);
  stride[0] = img->get_image_stride(0);

  pixels[1] = img->get_image_plane_at_pos_any_depth(1,xP/SubWidthC,yP/SubHeightC);
  stride[1] = img->get_image_stride(1);

  pixels[2] = img->get_image_plane_at_pos_any_depth(2,xP/SubWidthC,yP/SubHeightC);
  stride[2] = img->get_image_stride(2);


  ALIGNED_16(int16_t) predSamplesL                 [2 /* LX */][MAX_CU_SIZE* MAX_CU_SIZE];
  ALIGNED_16(int16_t) predSamplesC[2 /* chroma */ ][2 /* LX */][MAX_CU_SIZE* MAX_CU_SIZE];

  //int xP = xC+xB;
  //int yP = yC+yB;

  int predFlag[2];
  predFlag[0] = vi->predFlag[0];
  predFlag[1] = vi->predFlag[1];

  const int bit_depth_L = sps->BitDepth_Y;
  const int bit_depth_C = sps->BitDepth_C;

  // Some encoders use bi-prediction with two similar MVs.
  // Identify this case and use only one MV.

  // do this only without weighted prediction, because the weights/offsets may be different
  if (pps->weighted_pred_flag==0) {
    if (predFlag[0] && predFlag[1]) {
      if (vi->mv[0].x == vi->mv[1].x &&
          vi->mv[0].y == vi->mv[1].y &&
          shdr->RefPicList[0][vi->refIdx[0]] ==
          shdr->RefPicList[1][vi->refIdx[1]]) {
        predFlag[1] = 0;
      }
    }
  }


  for (int l=0;l<2;l++) {
    if (predFlag[l]) {
      // 8.5.3.2.1

      if (vi->refIdx[l] >= MAX_NUM_REF_PICS) {
        img->integrity = INTEGRITY_DECODING_ERRORS;
        ctx->add_warning(DE265_WARNING_NONEXISTING_REFERENCE_PICTURE_ACCESSED, false);
        return;
      }

      const de265_image* refPic = ctx->get_image(shdr->RefPicList[l][vi->refIdx[l]]);

      logtrace(LogMotion, "refIdx: %d -> dpb[%d]\n", vi->refIdx[l], shdr->RefPicList[l][vi->refIdx[l]]);

      if (!refPic || refPic->PicState == UnusedForReference) {
        img->integrity = INTEGRITY_DECODING_ERRORS;
        ctx->add_warning(DE265_WARNING_NONEXISTING_REFERENCE_PICTURE_ACCESSED, false);

        // TODO: fill predSamplesC with black or grey
      }
      else {
        // 8.5.3.2.2

        logtrace(LogMotion,"do MC: L%d,MV=%d;%d RefPOC=%d\n",
                 l,vi->mv[l].x,vi->mv[l].y,refPic->PicOrderCntVal);


        // TODO: must predSamples stride really be nCS or can it be somthing smaller like nPbW?

        if (img->high_bit_depth(0)) {
          mc_luma(ctx, sps, vi->mv[l].x, vi->mv[l].y, xP,yP,
                  predSamplesL[l],nCS,
                  (const uint16_t*)refPic->get_image_plane(0),
                  refPic->get_luma_stride(), nPbW,nPbH, bit_depth_L);
        }
        else {
          mc_luma(ctx, sps, vi->mv[l].x, vi->mv[l].y, xP,yP,
                  predSamplesL[l],nCS,
                  (const uint8_t*)refPic->get_image_plane(0),
                  refPic->get_luma_stride(), nPbW,nPbH, bit_depth_L);
        }

        if (img->high_bit_depth(1)) {
          mc_chroma(ctx, sps, vi->mv[l].x, vi->mv[l].y, xP,yP,
                    predSamplesC[0][l],nCS, (const uint16_t*)refPic->get_image_plane(1),
                    refPic->get_chroma_stride(), nPbW/SubWidthC,nPbH/SubHeightC, bit_depth_C);
          mc_chroma(ctx, sps, vi->mv[l].x, vi->mv[l].y, xP,yP,
                    predSamplesC[1][l],nCS, (const uint16_t*)refPic->get_image_plane(2),
                    refPic->get_chroma_stride(), nPbW/SubWidthC,nPbH/SubHeightC, bit_depth_C);
        }
        else {
          mc_chroma(ctx, sps, vi->mv[l].x, vi->mv[l].y, xP,yP,
                    predSamplesC[0][l],nCS, (const uint8_t*)refPic->get_image_plane(1),
                    refPic->get_chroma_stride(), nPbW/SubWidthC,nPbH/SubHeightC, bit_depth_C);
          mc_chroma(ctx, sps, vi->mv[l].x, vi->mv[l].y, xP,yP,
                    predSamplesC[1][l],nCS, (const uint8_t*)refPic->get_image_plane(2),
                    refPic->get_chroma_stride(), nPbW/SubWidthC,nPbH/SubHeightC, bit_depth_C);
        }
      }
    }
  }


  // weighted sample prediction  (8.5.3.2.3)

  const int shift1_L = libde265_max(2,14-sps->BitDepth_Y);
  const int offset_shift1_L = img->get_sps().WpOffsetBdShiftY;
  const int shift1_C = libde265_max(2,14-sps->BitDepth_C);
  const int offset_shift1_C = img->get_sps().WpOffsetBdShiftC;

  /*
  const int shift1_L = 14-img->sps.BitDepth_Y;
  const int offset_shift1_L = img->sps.BitDepth_Y-8;
  const int shift1_C = 14-img->sps.BitDepth_C;
  const int offset_shift1_C = img->sps.BitDepth_C-8;
  */

  /*
  if (0)
  printf("%d/%d %d/%d %d/%d %d/%d\n",
         shift1_L,
         Nshift1_L,
         offset_shift1_L,
         Noffset_shift1_L,
         shift1_C,
         Nshift1_C,
         offset_shift1_C,
         Noffset_shift1_C);

  assert(shift1_L==
         Nshift1_L);
  assert(offset_shift1_L==
         Noffset_shift1_L);
  assert(shift1_C==
         Nshift1_C);
  assert(offset_shift1_C==
         Noffset_shift1_C);
  */


  logtrace(LogMotion,"predFlags (modified): %d %d\n", predFlag[0], predFlag[1]);

  if (shdr->slice_type == SLICE_TYPE_P) {
    if (pps->weighted_pred_flag==0) {
      if (predFlag[0]==1 && predFlag[1]==0) {
        ctx->acceleration.put_unweighted_pred(pixels[0], stride[0],
                                              predSamplesL[0],nCS, nPbW,nPbH, bit_depth_L);
        ctx->acceleration.put_unweighted_pred(pixels[1], stride[1],
                                              predSamplesC[0][0],nCS,
                                              nPbW/SubWidthC,nPbH/SubHeightC, bit_depth_C);
        ctx->acceleration.put_unweighted_pred(pixels[2], stride[2],
                                              predSamplesC[1][0],nCS,
                                              nPbW/SubWidthC,nPbH/SubHeightC, bit_depth_C);
      }
      else {
        ctx->add_warning(DE265_WARNING_BOTH_PREDFLAGS_ZERO, false);
        img->integrity = INTEGRITY_DECODING_ERRORS;
      }
    }
    else {
      // weighted prediction

      if (predFlag[0]==1 && predFlag[1]==0) {

        int refIdx0 = vi->refIdx[0];

        int luma_log2WD   = shdr->luma_log2_weight_denom + shift1_L;
        int chroma_log2WD = shdr->ChromaLog2WeightDenom  + shift1_C;

        int luma_w0 = shdr->LumaWeight[0][refIdx0];
        int luma_o0 = shdr->luma_offset[0][refIdx0] * (1<<(offset_shift1_L));

        int chroma0_w0 = shdr->ChromaWeight[0][refIdx0][0];
        int chroma0_o0 = shdr->ChromaOffset[0][refIdx0][0] * (1<<(offset_shift1_C));
        int chroma1_w0 = shdr->ChromaWeight[0][refIdx0][1];
        int chroma1_o0 = shdr->ChromaOffset[0][refIdx0][1] * (1<<(offset_shift1_C));

        logtrace(LogMotion,"weighted-0 [%d] %d %d %d  %dx%d\n", refIdx0, luma_log2WD-6,luma_w0,luma_o0,nPbW,nPbH);

        ctx->acceleration.put_weighted_pred(pixels[0], stride[0],
                                            predSamplesL[0],nCS, nPbW,nPbH,
                                            luma_w0, luma_o0, luma_log2WD, bit_depth_L);
        ctx->acceleration.put_weighted_pred(pixels[1], stride[1],
                                            predSamplesC[0][0],nCS, nPbW/SubWidthC,nPbH/SubHeightC,
                                            chroma0_w0, chroma0_o0, chroma_log2WD, bit_depth_C);
        ctx->acceleration.put_weighted_pred(pixels[2], stride[2],
                                            predSamplesC[1][0],nCS, nPbW/SubWidthC,nPbH/SubHeightC,
                                            chroma1_w0, chroma1_o0, chroma_log2WD, bit_depth_C);
      }
      else {
        ctx->add_warning(DE265_WARNING_BOTH_PREDFLAGS_ZERO, false);
        img->integrity = INTEGRITY_DECODING_ERRORS;
      }
    }
  }
  else {
    assert(shdr->slice_type == SLICE_TYPE_B);

    if (predFlag[0]==1 && predFlag[1]==1) {
      if (pps->weighted_bipred_flag==0) {
        //const int shift2  = 15-8; // TODO: real bit depth
        //const int offset2 = 1<<(shift2-1);

        int16_t* in0 = predSamplesL[0];
        int16_t* in1 = predSamplesL[1];

        ctx->acceleration.put_weighted_pred_avg(pixels[0], stride[0],
                                                in0,in1, nCS, nPbW, nPbH, bit_depth_L);

        int16_t* in00 = predSamplesC[0][0];
        int16_t* in01 = predSamplesC[0][1];
        int16_t* in10 = predSamplesC[1][0];
        int16_t* in11 = predSamplesC[1][1];

        ctx->acceleration.put_weighted_pred_avg(pixels[1], stride[1],
                                                in00,in01, nCS,
                                                nPbW/SubWidthC, nPbH/SubHeightC, bit_depth_C);
        ctx->acceleration.put_weighted_pred_avg(pixels[2], stride[2],
                                                in10,in11, nCS,
                                                nPbW/SubWidthC, nPbH/SubHeightC, bit_depth_C);
      }
      else {
        // weighted prediction

        int refIdx0 = vi->refIdx[0];
        int refIdx1 = vi->refIdx[1];

        int luma_log2WD   = shdr->luma_log2_weight_denom + shift1_L;
        int chroma_log2WD = shdr->ChromaLog2WeightDenom + shift1_C;

        int luma_w0 = shdr->LumaWeight[0][refIdx0];
        int luma_o0 = shdr->luma_offset[0][refIdx0] * (1<<(offset_shift1_L));
        int luma_w1 = shdr->LumaWeight[1][refIdx1];
        int luma_o1 = shdr->luma_offset[1][refIdx1] * (1<<(offset_shift1_L));

        int chroma0_w0 = shdr->ChromaWeight[0][refIdx0][0];
        int chroma0_o0 = shdr->ChromaOffset[0][refIdx0][0] * (1<<(offset_shift1_C));
        int chroma1_w0 = shdr->ChromaWeight[0][refIdx0][1];
        int chroma1_o0 = shdr->ChromaOffset[0][refIdx0][1] * (1<<(offset_shift1_C));
        int chroma0_w1 = shdr->ChromaWeight[1][refIdx1][0];
        int chroma0_o1 = shdr->ChromaOffset[1][refIdx1][0] * (1<<(offset_shift1_C));
        int chroma1_w1 = shdr->ChromaWeight[1][refIdx1][1];
        int chroma1_o1 = shdr->ChromaOffset[1][refIdx1][1] * (1<<(offset_shift1_C));

        logtrace(LogMotion,"weighted-BI-0 [%d] %d %d %d  %dx%d\n", refIdx0, luma_log2WD-6,luma_w0,luma_o0,nPbW,nPbH);
        logtrace(LogMotion,"weighted-BI-1 [%d] %d %d %d  %dx%d\n", refIdx1, luma_log2WD-6,luma_w1,luma_o1,nPbW,nPbH);

        int16_t* in0 = predSamplesL[0];
        int16_t* in1 = predSamplesL[1];

        ctx->acceleration.put_weighted_bipred(pixels[0], stride[0],
                                              in0,in1, nCS, nPbW, nPbH,
                                              luma_w0,luma_o0,
                                              luma_w1,luma_o1,
                                              luma_log2WD, bit_depth_L);

        int16_t* in00 = predSamplesC[0][0];
        int16_t* in01 = predSamplesC[0][1];
        int16_t* in10 = predSamplesC[1][0];
        int16_t* in11 = predSamplesC[1][1];

        ctx->acceleration.put_weighted_bipred(pixels[1], stride[1],
                                              in00,in01, nCS, nPbW/SubWidthC, nPbH/SubHeightC,
                                              chroma0_w0,chroma0_o0,
                                              chroma0_w1,chroma0_o1,
                                              chroma_log2WD, bit_depth_C);
        ctx->acceleration.put_weighted_bipred(pixels[2], stride[2],
                                              in10,in11, nCS, nPbW/SubWidthC, nPbH/SubHeightC,
                                              chroma1_w0,chroma1_o0,
                                              chroma1_w1,chroma1_o1,
                                              chroma_log2WD, bit_depth_C);
      }
    }
    else if (predFlag[0]==1 || predFlag[1]==1) {
      int l = predFlag[0] ? 0 : 1;

      if (pps->weighted_bipred_flag==0) {
        ctx->acceleration.put_unweighted_pred(pixels[0], stride[0],
                                              predSamplesL[l],nCS, nPbW,nPbH, bit_depth_L);
        ctx->acceleration.put_unweighted_pred(pixels[1], stride[1],
                                              predSamplesC[0][l],nCS,
                                              nPbW/SubWidthC,nPbH/SubHeightC, bit_depth_C);
        ctx->acceleration.put_unweighted_pred(pixels[2], stride[2],
                                              predSamplesC[1][l],nCS,
                                              nPbW/SubWidthC,nPbH/SubHeightC, bit_depth_C);
      }
      else {
        int refIdx = vi->refIdx[l];

        int luma_log2WD   = shdr->luma_log2_weight_denom + shift1_L;
        int chroma_log2WD = shdr->ChromaLog2WeightDenom  + shift1_C;

        int luma_w = shdr->LumaWeight[l][refIdx];
        int luma_o = shdr->luma_offset[l][refIdx] * (1<<(offset_shift1_L));

        int chroma0_w = shdr->ChromaWeight[l][refIdx][0];
        int chroma0_o = shdr->ChromaOffset[l][refIdx][0] * (1<<(offset_shift1_C));
        int chroma1_w = shdr->ChromaWeight[l][refIdx][1];
        int chroma1_o = shdr->ChromaOffset[l][refIdx][1] * (1<<(offset_shift1_C));

        logtrace(LogMotion,"weighted-B-L%d [%d] %d %d %d  %dx%d\n", l, refIdx, luma_log2WD-6,luma_w,luma_o,nPbW,nPbH);

        ctx->acceleration.put_weighted_pred(pixels[0], stride[0],
                                            predSamplesL[l],nCS, nPbW,nPbH,
                                            luma_w, luma_o, luma_log2WD, bit_depth_L);
        ctx->acceleration.put_weighted_pred(pixels[1], stride[1],
                                            predSamplesC[0][l],nCS,
                                            nPbW/SubWidthC,nPbH/SubHeightC,
                                            chroma0_w, chroma0_o, chroma_log2WD, bit_depth_C);
        ctx->acceleration.put_weighted_pred(pixels[2], stride[2],
                                            predSamplesC[1][l],nCS,
                                            nPbW/SubWidthC,nPbH/SubHeightC,
                                            chroma1_w, chroma1_o, chroma_log2WD, bit_depth_C);
      }
    }
    else {
      // TODO: check why it can actually happen that both predFlags[] are false.
      // For now, we ignore this and continue decoding.

      ctx->add_warning(DE265_WARNING_BOTH_PREDFLAGS_ZERO, false);
      img->integrity = INTEGRITY_DECODING_ERRORS;
    }
  }

#if defined(DE265_LOG_TRACE) && 0
  logtrace(LogTransform,"MC pixels (luma), position %d %d:\n", xP,yP);

  for (int y=0;y<nPbH;y++) {
    logtrace(LogTransform,"MC-y-%d-%d ",xP,yP+y);

    for (int x=0;x<nPbW;x++) {
      logtrace(LogTransform,"*%02x ", pixels[0][x+y*stride[0]]);
    }

    logtrace(LogTransform,"*\n");
  }


  logtrace(LogTransform,"MC pixels (chroma cb), position %d %d:\n", xP/2,yP/2);

  for (int y=0;y<nPbH/2;y++) {
    logtrace(LogTransform,"MC-cb-%d-%d ",xP/2,yP/2+y);

    for (int x=0;x<nPbW/2;x++) {
      logtrace(LogTransform,"*%02x ", pixels[1][x+y*stride[1]]);
    }

    logtrace(LogTransform,"*\n");
  }


  logtrace(LogTransform,"MC pixels (chroma cr), position %d %d:\n", xP/2,yP/2);

  for (int y=0;y<nPbH/2;y++) {
    logtrace(LogTransform,"MC-cr-%d-%d ",xP/2,yP/2+y);

    for (int x=0;x<nPbW/2;x++) {
      logtrace(LogTransform,"*%02x ", pixels[2][x+y*stride[2]]);
    }

    logtrace(LogTransform,"*\n");
  }
#endif
}