  void expectWriteNewSessionTicket() override {
    std::string appParams("APP params");
    server->setEarlyDataAppParamsFunctions(
        [](const folly::Optional<std::string>&, const Buf&) { return false; },
        [=]() -> Buf { return folly::IOBuf::copyBuffer(appParams); });
    EXPECT_CALL(*getFakeHandshakeLayer(), writeNewSessionTicket(_))
        .WillOnce(Invoke([=](const AppToken& appToken) {
          auto& params = appToken.transportParams.parameters;

          auto initialMaxData = *getIntegerParameter(
              TransportParameterId::initial_max_data, params);
          EXPECT_EQ(
              initialMaxData,
              server->getConn()
                  .transportSettings.advertisedInitialConnectionWindowSize);

          auto initialMaxStreamDataBidiLocal = *getIntegerParameter(
              TransportParameterId::initial_max_stream_data_bidi_local, params);
          auto initialMaxStreamDataBidiRemote = *getIntegerParameter(
              TransportParameterId::initial_max_stream_data_bidi_remote,
              params);
          auto initialMaxStreamDataUni = *getIntegerParameter(
              TransportParameterId::initial_max_stream_data_bidi_remote,
              params);
          EXPECT_EQ(
              initialMaxStreamDataBidiLocal,
              server->getConn()
                  .transportSettings
                  .advertisedInitialBidiLocalStreamWindowSize);
          EXPECT_EQ(
              initialMaxStreamDataBidiRemote,
              server->getConn()
                  .transportSettings
                  .advertisedInitialBidiRemoteStreamWindowSize);
          EXPECT_EQ(
              initialMaxStreamDataUni,
              server->getConn()
                  .transportSettings.advertisedInitialUniStreamWindowSize);

          auto initialMaxStreamsBidi = *getIntegerParameter(
              TransportParameterId::initial_max_streams_bidi, params);
          auto initialMaxStreamsUni = *getIntegerParameter(
              TransportParameterId::initial_max_streams_uni, params);
          EXPECT_EQ(
              initialMaxStreamsBidi,
              server->getConn()
                  .transportSettings.advertisedInitialMaxStreamsBidi);
          EXPECT_EQ(
              initialMaxStreamsUni,
              server->getConn()
                  .transportSettings.advertisedInitialMaxStreamsUni);

          auto maxRecvPacketSize = *getIntegerParameter(
              TransportParameterId::max_packet_size, params);
          EXPECT_EQ(
              maxRecvPacketSize,
              server->getConn().transportSettings.maxRecvPacketSize);

          EXPECT_THAT(
              appToken.sourceAddresses, ContainerEq(expectedSourceToken_));

          EXPECT_TRUE(folly::IOBufEqualTo()(
              appToken.appParams, folly::IOBuf::copyBuffer(appParams)));
        }));
  }