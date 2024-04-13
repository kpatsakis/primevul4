void motion_vectors_and_ref_indices(base_context* ctx,
                                    const slice_segment_header* shdr,
                                    de265_image* img,
                                    const PBMotionCoding& motion,
                                    int xC,int yC, int xB,int yB, int nCS, int nPbW,int nPbH,
                                    int partIdx,
                                    PBMotion* out_vi)
{
  //slice_segment_header* shdr = tctx->shdr;

  int xP = xC+xB;
  int yP = yC+yB;

  enum PredMode predMode = img->get_pred_mode(xC,yC);

  if (predMode == MODE_SKIP ||
      (predMode == MODE_INTER && motion.merge_flag))
    {
      derive_luma_motion_merge_mode(ctx,shdr,img,
                                    xC,yC, xP,yP, nCS,nPbW,nPbH, partIdx,
                                    motion.merge_idx, out_vi);

      logMV(xP,yP,nPbW,nPbH, "merge_mode", out_vi);
    }
  else {
    int mvdL[2][2];
    MotionVector mvpL[2];

    for (int l=0;l<2;l++) {
      // 1.

      enum InterPredIdc inter_pred_idc = (enum InterPredIdc)motion.inter_pred_idc;

      if (inter_pred_idc == PRED_BI ||
          (inter_pred_idc == PRED_L0 && l==0) ||
          (inter_pred_idc == PRED_L1 && l==1)) {
        out_vi->refIdx[l] = motion.refIdx[l];
        out_vi->predFlag[l] = 1;
      }
      else {
        out_vi->refIdx[l] = -1;
        out_vi->predFlag[l] = 0;
      }

      // 2.

      mvdL[l][0] = motion.mvd[l][0];
      mvdL[l][1] = motion.mvd[l][1];


      if (out_vi->predFlag[l]) {
        // 3.

        mvpL[l] = luma_motion_vector_prediction(ctx,shdr,img,motion,
                                                xC,yC,nCS,xP,yP, nPbW,nPbH, l,
                                                out_vi->refIdx[l], partIdx);

        // 4.

        int32_t x = (mvpL[l].x + mvdL[l][0] + 0x10000) & 0xFFFF;
        int32_t y = (mvpL[l].y + mvdL[l][1] + 0x10000) & 0xFFFF;

        out_vi->mv[l].x = (x>=0x8000) ? x-0x10000 : x;
        out_vi->mv[l].y = (y>=0x8000) ? y-0x10000 : y;
      }
    }

    logMV(xP,yP,nPbW,nPbH, "mvp", out_vi);
  }
}