MotionVector luma_motion_vector_prediction(base_context* ctx,
                                           const slice_segment_header* shdr,
                                           de265_image* img,
                                           const PBMotionCoding& motion,
                                           int xC,int yC,int nCS,int xP,int yP,
                                           int nPbW,int nPbH, int l,
                                           int refIdx, int partIdx)
{
  MotionVector mvpList[2];

  fill_luma_motion_vector_predictors(ctx, shdr, img,
                                     xC,yC,nCS,xP,yP,
                                     nPbW, nPbH, l, refIdx, partIdx,
                                     mvpList);

  // select predictor according to mvp_lX_flag

  return mvpList[ l ? motion.mvp_l1_flag : motion.mvp_l0_flag ];
}