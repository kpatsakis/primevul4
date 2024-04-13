void logMV(int x0,int y0,int nPbW,int nPbH, const char* mode,const PBMotion* mv)
{
  int pred0 = mv->predFlag[0];
  int pred1 = mv->predFlag[1];

  logtrace(LogMotion,
           "*MV %d;%d [%d;%d] %s: (%d) %d;%d @%d   (%d) %d;%d @%d\n", x0,y0,nPbW,nPbH,mode,
           pred0,
           pred0 ? mv->mv[0].x : 0,pred0 ? mv->mv[0].y : 0, pred0 ? mv->refIdx[0] : 0,
           pred1,
           pred1 ? mv->mv[1].x : 0,pred1 ? mv->mv[1].y : 0, pred1 ? mv->refIdx[1] : 0);
}