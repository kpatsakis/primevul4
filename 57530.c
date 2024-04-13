onig_region_init(OnigRegion* region)
{
  region->num_regs     = 0;
  region->allocated    = 0;
  region->beg          = (int* )0;
  region->end          = (int* )0;
  region->history_root = (OnigCaptureTreeNode* )0;
}
