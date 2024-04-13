bool SingleComponentLSScan::WriteMCU(void)
{
#if ACCUSOFT_CODE
  int lines             = m_ulRemaining[0]; // total number of MCU lines processed.
  UBYTE preshift        = m_ucLowBit + FractionalColorBitsOf();
  struct Line *line     = CurrentLine(0);
  
  assert(m_ucCount == 1);

  //
  // A "MCU" in respect to the code organization is eight lines.
  if (lines > 8) {
    lines = 8;
  }
  m_ulRemaining[0] -= lines;
  assert(lines > 0);

  // Loop over lines and columns
  do {
    LONG length = m_ulWidth[0];
    LONG *lp    = line->m_pData;

    BeginWriteMCU(m_Stream.ByteStreamOf()); // MCU is a single line.
    StartLine(0);
    do {
      LONG a,b,c,d,x; // neighbouring values.
      LONG d1,d2,d3;  // local gradients.
      
      GetContext(0,a,b,c,d);
      x   = *lp >> preshift;
      
      d1  = d - b;    // compute local gradients
      d2  = b - c;
      d3  = c - a;

      if (isRunMode(d1,d2,d3)) {
        LONG runval = a;
        LONG runcnt = 0;
        do {
          x  = *lp >> preshift;
          if (x - runval < -m_lNear || x - runval > m_lNear)
            break;
          // Update so that the next process gets the correct value.
          // Also updates the line pointers.
          UpdateContext(0,runval);
        } while(lp++,runcnt++,--length);
        // Encode the run. Depends on whether the run was interrupted
        // by the end of the line.
        EncodeRun(runcnt,length == 0,m_lRunIndex[0]);
        // Continue the encoding of the end of the run if there are more
        // samples to encode.
        if (length) {
          bool negative; // the sign variable
          bool rtype;    // run interruption type
          LONG errval;   // the prediction error
          LONG merr;     // the mapped error (symbol)
          LONG rx;       // the reconstructed value
          UBYTE k;       // golomb parameter
          // Get the neighbourhood.
          GetContext(0,a,b,c,d);
          // Get the prediction mode.
          rtype  = InterruptedPredictionMode(negative,a,b);
          // Compute the error value.
          errval = x - ((rtype)?(a):(b));
          if (negative)
            errval = -errval;
          // Quantize the error.
          errval = QuantizePredictionError(errval);
          // Compute the reconstructed value.
          rx     = Reconstruct(negative,rtype?a:b,errval);
          // Update so that the next process gets the correct value.
          UpdateContext(0,rx);
          // Get the golomb parameter for run interruption coding.
          k      = GolombParameter(rtype);
          // Map the error into a symbol.
          merr   = ErrorMapping(errval,ErrorMappingOffset(rtype,errval != 0,k)) - rtype;
          // Golomb-coding of the error.
          GolombCode(k,merr,m_lLimit - m_lJ[m_lRunIndex[0]] - 1);
          // Update the variables of the run mode.
          UpdateState(rtype,errval);
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
        // Quantize the gradients.
        d1     = QuantizedGradient(d1);
        d2     = QuantizedGradient(d2);
        d3     = QuantizedGradient(d3);
        // Compute the context.
        ctxt   = Context(negative,d1,d2,d3); 
        // Compute the predicted value.
        px     = Predict(a,b,c);
        // Correct the prediction.
        px     = CorrectPrediction(ctxt,negative,px);
        // Compute the error value.
        errval = x - px;
        if (negative)
          errval = -errval;
        // Quantize the prediction error if NEAR > 0
        errval = QuantizePredictionError(errval);
        // Compute the reconstructed value.
        rx     = Reconstruct(negative,px,errval);
        // Update so that the next process gets the correct value.
        UpdateContext(0,rx);
        // Compute the golomb parameter k from the context.
        k      = GolombParameter(ctxt);
        // Map the error into a symbol
        merr   = ErrorMapping(errval,ErrorMappingOffset(ctxt,k));
        // Golomb-coding of the error.
        GolombCode(k,merr,m_lLimit);
        // Update the variables.
        UpdateState(ctxt,errval);
      }
    } while(++lp,--length);
    EndLine(0);
    line = line->m_pNext;
  } while(--lines);
#endif
  return false;
}