wkbReadPoint(wkbObj *w)
{
  pointObj p;
  wkbReadPointP(w, &p);
  return p;
}
