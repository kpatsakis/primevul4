void get_merge_candidate_list(base_context* ctx,
                              const slice_segment_header* shdr,
                              de265_image* img,
                              int xC,int yC, int xP,int yP,
                              int nCS, int nPbW,int nPbH, int partIdx,
                              PBMotion* mergeCandList)
{
  int max_merge_idx = 5-shdr->five_minus_max_num_merge_cand -1;

  get_merge_candidate_list_without_step_9(ctx, shdr,
                                          MotionVectorAccess_de265_image(img), img,
                                          xC,yC,xP,yP,nCS,nPbW,nPbH, partIdx,
                                          max_merge_idx, mergeCandList);

  // 9. for encoder: modify all merge candidates

  for (int i=0;i<=max_merge_idx;i++) {
    if (mergeCandList[i].predFlag[0] &&
        mergeCandList[i].predFlag[1] &&
        nPbW+nPbH==12)
      {
        mergeCandList[i].refIdx[1]   = -1;
        mergeCandList[i].predFlag[1] = 0;
      }
  }
}