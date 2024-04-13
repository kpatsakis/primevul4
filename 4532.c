  virtual void setupClientReadCodec() {
    FizzCryptoFactory cryptoFactory;
    clientReadCodec = std::make_unique<QuicReadCodec>(QuicNodeType::Client);
    clientReadCodec->setClientConnectionId(*clientConnectionId);
    clientReadCodec->setInitialReadCipher(cryptoFactory.getServerInitialCipher(
        *initialDestinationConnectionId, QuicVersion::MVFST));
    clientReadCodec->setInitialHeaderCipher(
        cryptoFactory.makeServerInitialHeaderCipher(
            *initialDestinationConnectionId, QuicVersion::MVFST));
    clientReadCodec->setCodecParameters(
        CodecParameters(kDefaultAckDelayExponent, QuicVersion::MVFST));
  }