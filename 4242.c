void derive_luma_motion_merge_mode(base_context* ctx,
                                   const slice_segment_header* shdr,
                                   de265_image* img,
                                   int xC,int yC, int xP,int yP,
                                   int nCS, int nPbW,int nPbH, int partIdx,
                                   int merge_idx,
                                   PBMotion* out_vi)
{
  PBMotion mergeCandList[5];

  get_merge_candidate_list_without_step_9(ctx, shdr,
                                          MotionVectorAccess_de265_image(img), img,
                                          xC,yC,xP,yP,nCS,nPbW,nPbH, partIdx,
                                          merge_idx, mergeCandList);


  *out_vi = mergeCandList[merge_idx];

  // 8.5.3.1.1 / 9.

  if (out_vi->predFlag[0] && out_vi->predFlag[1] && nPbW+nPbH==12) {
    out_vi->refIdx[1] = -1;
    out_vi->predFlag[1] = 0;
  }
}