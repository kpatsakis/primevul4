  void setOneRttKeys() {
    // Mimic ServerHandshake behavior.
    // oneRttWriteCipher would already be set during ReportEarlyHandshakeSuccess
    if (!allowZeroRttKeys_) {
      oneRttWriteCipher_ = createNoOpAead();
      oneRttWriteHeaderCipher_ = createNoOpHeaderCipher();
    }
    oneRttReadCipher_ = createNoOpAead();
    oneRttReadHeaderCipher_ = createNoOpHeaderCipher();
  }