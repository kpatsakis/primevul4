  void doHandshake(std::unique_ptr<IOBuf> data, EncryptionLevel) override {
    IOBufEqualTo eq;
    auto chlo = folly::IOBuf::copyBuffer("CHLO");
    auto clientFinished = IOBuf::copyBuffer("FINISHED");
    if (eq(data, chlo)) {
      if (chloSync_) {
        // Do NOT invoke onCryptoEventAvailable callback
        // Fall through and let the ServerStateMachine to process the event
        writeDataToQuicStream(
            *getCryptoStream(*conn_.cryptoState, EncryptionLevel::Initial),
            IOBuf::copyBuffer("SHLO"));
        if (allowZeroRttKeys_) {
          validateAndUpdateSourceToken(conn_, sourceAddrs_);
          phase_ = Phase::KeysDerived;
          setEarlyKeys();
        }
        setHandshakeKeys();
      } else {
        // Asynchronously schedule the callback
        executor_->add([&] {
          writeDataToQuicStream(
              *getCryptoStream(*conn_.cryptoState, EncryptionLevel::Initial),
              IOBuf::copyBuffer("SHLO"));
          if (allowZeroRttKeys_) {
            validateAndUpdateSourceToken(conn_, sourceAddrs_);
            phase_ = Phase::KeysDerived;
            setEarlyKeys();
          }
          setHandshakeKeys();
          if (callback_) {
            callback_->onCryptoEventAvailable();
          }
        });
      }
    } else if (eq(data, clientFinished)) {
      if (cfinSync_) {
        // Do NOT invoke onCryptoEventAvailable callback
        // Fall through and let the ServerStateMachine to process the event
        setOneRttKeys();
        phase_ = Phase::Established;
        handshakeDone_ = true;
      } else {
        // Asynchronously schedule the callback
        executor_->add([&] {
          setOneRttKeys();
          phase_ = Phase::Established;
          handshakeDone_ = true;
          if (callback_) {
            callback_->onCryptoEventAvailable();
          }
        });
      }
    }
  }