


void connectToMQTT();
void publishAll();
void forceMqttReconnect();


void publishEffectState();
void publishAniState();
void publishV1State();
void publishV2State();
void publishH1State();
void publishH2State();
void publishVsState();
void publishHsState();
void publishEfxTimeState();
void publishAniTimeState();
void publishAniDepthState();

// Sensor publishing
void publishIpAddress();
void publishUptime();
void publishRssi();
void publishHeapMemory();
void publishBrightness();
void publishLastNtpSync();
void publishTemperature();
void publishSystemLoad();
void publishSensorStates();
