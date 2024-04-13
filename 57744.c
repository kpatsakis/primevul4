static void CompressClusterFit(const size_t count,
  const DDSVector4 *points, const ssize_t *map, const DDSVector3 principle,
  const DDSVector4 metric, DDSVector3 *start, DDSVector3 *end,
  unsigned char *indices)
{
  DDSVector3
    axis;

  DDSVector4
    grid,
    gridrcp,
    half,
    onethird_onethird2,
    pointsWeights[16],
    two,
    twonineths,
    twothirds_twothirds2,
    xSumwSum;

  float
    bestError = 1e+37f;

  size_t
    bestIteration = 0,
    besti = 0,
    bestj = 0,
    bestk = 0,
    iterationIndex;

  ssize_t
    i;

  unsigned char
    *o,
    order[128],
    unordered[16];

  VectorInit(half,0.5f);
  VectorInit(two,2.0f);

  VectorInit(onethird_onethird2,1.0f/3.0f);
  onethird_onethird2.w = 1.0f/9.0f;
  VectorInit(twothirds_twothirds2,2.0f/3.0f);
  twothirds_twothirds2.w = 4.0f/9.0f;
  VectorInit(twonineths,2.0f/9.0f);

  grid.x = 31.0f;
  grid.y = 63.0f;
  grid.z = 31.0f;
  grid.w = 0.0f;

  gridrcp.x = 1.0f/31.0f;
  gridrcp.y = 1.0f/63.0f;
  gridrcp.z = 1.0f/31.0f;
  gridrcp.w = 0.0f;

  xSumwSum.x = 0.0f;
  xSumwSum.y = 0.0f;
  xSumwSum.z = 0.0f;
  xSumwSum.w = 0.0f;

  ConstructOrdering(count,points,principle,pointsWeights,&xSumwSum,order,0);

  for (iterationIndex = 0;;)
  {
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(dynamic,1) \
    num_threads(GetMagickResourceLimit(ThreadResource))
#endif
    for (i=0; i < (ssize_t) count; i++)
    {
      DDSVector4
        part0,
        part1,
        part2;

      size_t
        ii,
        j,
        k,
        kmin;

      VectorInit(part0,0.0f);
      for(ii=0; ii < (size_t) i; ii++)
        VectorAdd(pointsWeights[ii],part0,&part0);

      VectorInit(part1,0.0f);
      for (j=(size_t) i;;)
      {
        if (j == 0)
          {
            VectorCopy44(pointsWeights[0],&part2);
            kmin = 1;
          }
          else
          {
            VectorInit(part2,0.0f);
            kmin = j;
          }

        for (k=kmin;;)
        {
          DDSVector4
            a,
            alpha2_sum,
            alphax_sum,
            alphabeta_sum,
            b,
            beta2_sum,
            betax_sum,
            e1,
            e2,
            factor,
            part3;

          float
            error;

          VectorSubtract(xSumwSum,part2,&part3);
          VectorSubtract(part3,part1,&part3);
          VectorSubtract(part3,part0,&part3);

          VectorMultiplyAdd(part1,twothirds_twothirds2,part0,&alphax_sum);
          VectorMultiplyAdd(part2,onethird_onethird2,alphax_sum,&alphax_sum);
          VectorInit(alpha2_sum,alphax_sum.w);

          VectorMultiplyAdd(part2,twothirds_twothirds2,part3,&betax_sum);
          VectorMultiplyAdd(part1,onethird_onethird2,betax_sum,&betax_sum);
          VectorInit(beta2_sum,betax_sum.w);

          VectorAdd(part1,part2,&alphabeta_sum);
          VectorInit(alphabeta_sum,alphabeta_sum.w);
          VectorMultiply(twonineths,alphabeta_sum,&alphabeta_sum);

          VectorMultiply(alpha2_sum,beta2_sum,&factor);
          VectorNegativeMultiplySubtract(alphabeta_sum,alphabeta_sum,factor,
            &factor);
          VectorReciprocal(factor,&factor);

          VectorMultiply(alphax_sum,beta2_sum,&a);
          VectorNegativeMultiplySubtract(betax_sum,alphabeta_sum,a,&a);
          VectorMultiply(a,factor,&a);

          VectorMultiply(betax_sum,alpha2_sum,&b);
          VectorNegativeMultiplySubtract(alphax_sum,alphabeta_sum,b,&b);
          VectorMultiply(b,factor,&b);

          VectorClamp(&a);
          VectorMultiplyAdd(grid,a,half,&a);
          VectorTruncate(&a);
          VectorMultiply(a,gridrcp,&a);

          VectorClamp(&b);
          VectorMultiplyAdd(grid,b,half,&b);
          VectorTruncate(&b);
          VectorMultiply(b,gridrcp,&b);

          VectorMultiply(b,b,&e1);
          VectorMultiply(e1,beta2_sum,&e1);
          VectorMultiply(a,a,&e2);
          VectorMultiplyAdd(e2,alpha2_sum,e1,&e1);

          VectorMultiply(a,b,&e2);
          VectorMultiply(e2,alphabeta_sum,&e2);
          VectorNegativeMultiplySubtract(a,alphax_sum,e2,&e2);
          VectorNegativeMultiplySubtract(b,betax_sum,e2,&e2);
          VectorMultiplyAdd(two,e2,e1,&e2);
          VectorMultiply(e2,metric,&e2);

          error = e2.x + e2.y + e2.z;

          if (error < bestError)
            {
#if defined(MAGICKCORE_OPENMP_SUPPORT)
              #pragma omp critical (DDS_CompressClusterFit)
#endif
              {
                if (error < bestError)
                  {
                    VectorCopy43(a,start);
                    VectorCopy43(b,end);
                    bestError = error;
                    besti = i;
                    bestj = j;
                    bestk = k;
                    bestIteration = iterationIndex;
                  }
              }
            }

          if (k == count)
            break;

          VectorAdd(pointsWeights[k],part2,&part2);
          k++;
        }

        if (j == count)
          break;

        VectorAdd(pointsWeights[j],part1,&part1);
        j++;
      }
    }

    if (bestIteration != iterationIndex)
      break;

    iterationIndex++;
    if (iterationIndex == 8)
      break;

    VectorSubtract3(*end,*start,&axis);
    if (ConstructOrdering(count,points,axis,pointsWeights,&xSumwSum,order,
      iterationIndex) == MagickFalse)
      break;
  }

  o = order + (16*bestIteration);

  for (i=0; i < (ssize_t) besti; i++)
    unordered[o[i]] = 0;
  for (i=besti; i < (ssize_t) bestj; i++)
    unordered[o[i]] = 2;
  for (i=bestj; i < (ssize_t) bestk; i++)
    unordered[o[i]] = 3;
  for (i=bestk; i < (ssize_t) count; i++)
    unordered[o[i]] = 1;

  RemapIndices(map,unordered,indices);
}
