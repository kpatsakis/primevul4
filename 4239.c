void logmvcand(const PBMotion& p)
{
  for (int v=0;v<2;v++) {
    if (p.predFlag[v]) {
      logtrace(LogMotion,"  %d: %s  %d;%d ref=%d\n", v, p.predFlag[v] ? "yes":"no ",
               p.mv[v].x,p.mv[v].y, p.refIdx[v]);
    } else {
      logtrace(LogMotion,"  %d: %s  --;-- ref=--\n", v, p.predFlag[v] ? "yes":"no ");
    }
  }
}