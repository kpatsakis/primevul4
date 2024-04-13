void decode_prediction_unit(base_context* ctx,
                            const slice_segment_header* shdr,
                            de265_image* img,
                            const PBMotionCoding& motion,
                            int xC,int yC, int xB,int yB, int nCS, int nPbW,int nPbH, int partIdx)
{
  logtrace(LogMotion,"decode_prediction_unit POC=%d %d;%d %dx%d\n",
           img->PicOrderCntVal, xC+xB,yC+yB, nPbW,nPbH);

  //slice_segment_header* shdr = tctx->shdr;

  // 1.

  PBMotion vi;
  motion_vectors_and_ref_indices(ctx, shdr, img, motion,
                                 xC,yC, xB,yB, nCS, nPbW,nPbH, partIdx, &vi);

  // 2.

  generate_inter_prediction_samples(ctx,shdr, img, xC,yC, xB,yB, nCS, nPbW,nPbH, &vi);


  img->set_mv_info(xC+xB,yC+yB,nPbW,nPbH, vi);
}