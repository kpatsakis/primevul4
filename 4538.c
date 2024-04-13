bool SampleInterleavedLSScan::WriteMCU(void)
{
#if ACCUSOFT_CODE
  int lines             = m_ulRemaining[0]; // total number of MCU lines processed.
  UBYTE preshift        = m_ucLowBit + FractionalColorBitsOf();
  struct Line *line[4];
  UBYTE cx;
  
  //
  // A "MCU" in respect to the code organization is eight lines.
  if (lines > 8) {
    lines = 8;
  }
  m_ulRemaining[0] -= lines;
  assert(lines > 0);
  assert(m_ucCount < 4);

  //
  // Fill the line pointers.
  for(cx = 0;cx < m_ucCount;cx++) {
    line[cx] = CurrentLine(cx);
  }

  // Loop over lines and columns
  do {
    LONG length = m_ulWidth[0];
    LONG *lp[4];

    // Get the line pointers and initialize the internal backup lines.
    for(cx = 0;cx < m_ucCount;cx++) {
      lp[cx] = line[cx]->m_pData;
      StartLine(cx);
    }
    //
    BeginWriteMCU(m_Stream.ByteStreamOf()); 
    do {
        LONG a[4],b[4],c[4],d[4]; // neighbouring values.
        LONG d1[4],d2[4],d3[4];   // local gradients.
        bool isrun = true;
      
        for(cx = 0;cx < m_ucCount;cx++) {
          GetContext(cx,a[cx],b[cx],c[cx],d[cx]);

          d1[cx]  = d[cx] - b[cx];    // compute local gradients
          d2[cx]  = b[cx] - c[cx];
          d3[cx]  = c[cx] - a[cx];

          //
          // Run mode only if the run condition is met for all components
          if (isrun && !isRunMode(d1[cx],d2[cx],d3[cx]))
            isrun = false;
        }
        
        if (isrun) {
          LONG runcnt = 0;
          do {
            //
            // Check whether the pixel is close enough to continue the run.
            for(cx = 0;cx < m_ucCount;cx++) {
              LONG x  = *lp[cx] >> preshift;
              if (x - a[cx] < -m_lNear || x - a[cx] > m_lNear)
                break;
            }
            if (cx < m_ucCount)
              break; // run ends.
            //
            // Update so that the next process gets the correct value.
            // Also updates the line pointers.
            for(cx = 0;cx < m_ucCount;cx++) {
              UpdateContext(cx,a[cx]);
              lp[cx]++;
            }
          } while(runcnt++,--length);
          //
          // Encode the run. Note that only a single run index is used here.
          EncodeRun(runcnt,length == 0,m_lRunIndex[0]);
          // Continue the encoding of the end of the run if there are more
          // samples to encode.
          if (length) {       
            bool negative; // the sign variable
            LONG errval;   // the prediction error
            LONG merr;     // the mapped error (symbol)
            LONG rx;       // the reconstructed value
            UBYTE k;       // golomb parameter
            //
            // The complete pixel in all components is now to be encoded.
            for(cx = 0;cx < m_ucCount;cx++) {
              // Get the neighbourhood.
              GetContext(cx,a[cx],b[cx],c[cx],d[cx]);
              // The prediction mode is always fixed, but the sign
              // has to be found.
              negative = a[cx] > b[cx];
              // Compute the error value.
              errval   = (*lp[cx]++ >> preshift) - b[cx];
              if (negative)
                errval = -errval;
              // Quantize the error.
              errval = QuantizePredictionError(errval);
              // Compute the reconstructed value.
              rx     = Reconstruct(negative,b[cx],errval);
              // Update so that the next process gets the correct value.
              UpdateContext(cx,rx);
              // Get the golomb parameter for run interruption coding.
              k      = GolombParameter(false);
              // Map the error into a symbol.
              merr   = ErrorMapping(errval,ErrorMappingOffset(false,errval != 0,k));
              // Golomb-coding of the error.
              GolombCode(k,merr,m_lLimit - m_lJ[m_lRunIndex[0]] - 1);
              // Update the variables of the run mode.
              UpdateState(false,errval);
            }
            // Update the run index now. This is not part of
            // EncodeRun because the non-reduced run-index is
            // required for the golomb coder length limit.
            if (m_lRunIndex[0] > 0)
                m_lRunIndex[0]--;
          } else break; // Line ended, abort the loop over the line.  
        } else { 
          UWORD ctxt;
          bool  negative; // the sign variable.
          LONG  px;       // the predicted variable.
          LONG  rx;       // the reconstructed value.
          LONG  errval;   // the error value.
          LONG  merr;     // the mapped error value.
          UBYTE k;        // the Golomb parameter.
          //
          for(cx = 0;cx < m_ucCount;cx++) {
            // Quantize the gradients.
            d1[cx]     = QuantizedGradient(d1[cx]);
            d2[cx]     = QuantizedGradient(d2[cx]);
            d3[cx]     = QuantizedGradient(d3[cx]);
            // Compute the context.
            ctxt   = Context(negative,d1[cx],d2[cx],d3[cx]); 
            // Compute the predicted value.
            px     = Predict(a[cx],b[cx],c[cx]);
            // Correct the prediction.
            px     = CorrectPrediction(ctxt,negative,px);
            // Compute the error value.
            errval = (*lp[cx]++ >> preshift) - px;
            if (negative)
              errval = -errval;
            // Quantize the prediction error if NEAR > 0
            errval = QuantizePredictionError(errval);
            // Compute the reconstructed value.
            rx     = Reconstruct(negative,px,errval);
            // Update so that the next process gets the correct value.
            UpdateContext(cx,rx);
            // Compute the golomb parameter k from the context.
            k      = GolombParameter(ctxt);
            // Map the error into a symbol
            merr   = ErrorMapping(errval,ErrorMappingOffset(ctxt,k));
            // Golomb-coding of the error.
            GolombCode(k,merr,m_lLimit);
            // Update the variables.
            UpdateState(ctxt,errval);
          }
        }
    } while(--length);
    //
    // Advance the line pointers.
    for(cx = 0;cx < m_ucCount;cx++) {
      EndLine(cx);
      line[cx] = line[cx]->m_pNext;
    }
    //
  } while(--lines);
#endif  
  return false;
}