#ifdef SBUS_module
/**
 * Tested with SP Racing f3 flight controller and iNav.
 * Still Beta, not tested in flight yet.
 * With this communication type, UART is set to 100000 8E2. Standard Arduino monitor is not supporting this speed, so don't bother. Use Putty.
 * 
 * Based on code of Pawel Spychalski:
 * https://quadmeup.com/generate-s-bus-with-arduino-in-a-simple-way/
 * 
 * For some reason, there is an error when trying to create pocket first, then send it by Serial.write(pocket);
 * If there is "\0" Serial function deos not send it. When trying to change data type or setting fixed buffer length, there is an error during compilation.
 * Maybe someone will find and fix the proble someday. For now, there are plenty of Serial.write() calls.
 */
#define RC_CHANNEL_MIN 990
#define RC_CHANNEL_MAX 2010

#define SBUS_MIN_OFFSET 173
#define SBUS_MID_OFFSET 992
#define SBUS_MAX_OFFSET 1811
#define SBUS_CHANNEL_NUMBER 16
#define SBUS_PACKET_LENGTH 25
#define SBUS_FRAME_HEADER 0x0f
#define SBUS_FRAME_FOOTER 0x00
#define SBUS_FRAME_FOOTER_V2 0x04
#define SBUS_STATE_FAILSAFE 0x08
#define SBUS_STATE_SIGNALLOSS 0x04
#define SBUS_UPDATE_RATE 15 //ms


#ifdef RX_module


void setup_module() {
    // module works on hardware serial - must be 100000/8E2
    //Serial.end();
    //Serial.begin(100000, SERIAL_8E2);
}

void send_servo_frame(){
    unsigned char packet[25];
    static int output[SBUS_CHANNEL_NUMBER] = {0};

    /*
     * Map 1000-2000 with middle at 1500 chanel values to
     * 173-1811 with middle at 992 S.BUS protocol requires
     */
    for (uint8_t i = 0; i < SBUS_CHANNEL_NUMBER; i++) {
        output[i] = map( (i < SERVO_CHANNELS ? (Servos[i] + SERVO_SHIFT) : 1500), RC_CHANNEL_MIN, RC_CHANNEL_MAX, SBUS_MIN_OFFSET, SBUS_MAX_OFFSET);
    } 

    uint8_t stateByte = 0x00;
    
    if (lost_frames > 12) {
        stateByte |= SBUS_STATE_SIGNALLOSS;
    }
    if (failsafe_state) {
        stateByte |= SBUS_STATE_FAILSAFE;
    }

    Serial.write(SBUS_FRAME_HEADER);

    Serial.write(output[0] & 0x07FF);
    Serial.write((output[0] & 0x07FF)>>8 | (output[1] & 0x07FF)<<3);

    Serial.write((output[1] & 0x07FF)>>5 | (output[2] & 0x07FF)<<6);
    Serial.write((output[2] & 0x07FF)>>2);
    Serial.write((output[2] & 0x07FF)>>10 | (output[3] & 0x07FF)<<1);
    Serial.write((output[3] & 0x07FF)>>7 | (output[4] & 0x07FF)<<4);
    Serial.write((output[4] & 0x07FF)>>4 | (output[5] & 0x07FF)<<7);
    Serial.write((output[5] & 0x07FF)>>1);
    Serial.write((output[5] & 0x07FF)>>9 | (output[6] & 0x07FF)<<2);
    Serial.write((output[6] & 0x07FF)>>6 | (output[7] & 0x07FF)<<5);
    Serial.write((output[7] & 0x07FF)>>3);
    Serial.write((output[8] & 0x07FF));
    Serial.write((output[8] & 0x07FF)>>8 | (output[9] & 0x07FF)<<3);
    Serial.write((output[9] & 0x07FF)>>5 | (output[10] & 0x07FF)<<6);  
    Serial.write((output[10] & 0x07FF)>>2);
    Serial.write((output[10] & 0x07FF)>>10 | (output[11] & 0x07FF)<<1);
    Serial.write((output[11] & 0x07FF)>>7 | (output[12] & 0x07FF)<<4);
    Serial.write((output[12] & 0x07FF)>>4 | (output[13] & 0x07FF)<<7);
    Serial.write((output[13] & 0x07FF)>>1);
    Serial.write((output[13] & 0x07FF)>>9 | (output[14] & 0x07FF)<<2);
    Serial.write((output[14] & 0x07FF)>>6 | (output[15] & 0x07FF)<<5);
    Serial.write((output[15] & 0x07FF)>>3);
    Serial.write (stateByte); //Flags byte
    Serial.write(SBUS_FRAME_FOOTER); //Footer

}

    /*
    packet[0] = SBUS_FRAME_HEADER; //Header
    packet[1] = (uint8_t) (output[0] & 0x07FF);
    packet[2] = (uint8_t) ((output[0] & 0x07FF)>>8 | (output[1] & 0x07FF)<<3);
    packet[3] = (uint8_t) ((output[1] & 0x07FF)>>5 | (output[2] & 0x07FF)<<6);
    packet[4] = (uint8_t) ((output[2] & 0x07FF)>>2);
    packet[5] = (uint8_t) ((output[2] & 0x07FF)>>10 | (output[3] & 0x07FF)<<1);
    packet[6] = (uint8_t) ((output[3] & 0x07FF)>>7 | (output[4] & 0x07FF)<<4);
    packet[7] = (uint8_t) ((output[4] & 0x07FF)>>4 | (output[5] & 0x07FF)<<7);
    packet[8] = (uint8_t) ((output[5] & 0x07FF)>>1);
    packet[9] = (uint8_t) ((output[5] & 0x07FF)>>9 | (output[6] & 0x07FF)<<2);
    packet[10] = (uint8_t) ((output[6] & 0x07FF)>>6 | (output[7] & 0x07FF)<<5);
    packet[11] = (uint8_t) ((output[7] & 0x07FF)>>3);
    packet[12] = (uint8_t) ((output[8] & 0x07FF));
    packet[13] = (uint8_t) ((output[8] & 0x07FF)>>8 | (output[9] & 0x07FF)<<3);
    packet[14] = (uint8_t) ((output[9] & 0x07FF)>>5 | (output[10] & 0x07FF)<<6);  
    packet[15] = (uint8_t) ((output[10] & 0x07FF)>>2);
    packet[16] = (uint8_t) ((output[10] & 0x07FF)>>10 | (output[11] & 0x07FF)<<1);
    packet[17] = (uint8_t) ((output[11] & 0x07FF)>>7 | (output[12] & 0x07FF)<<4);
    packet[18] = (uint8_t) ((output[12] & 0x07FF)>>4 | (output[13] & 0x07FF)<<7);
    packet[19] = (uint8_t) ((output[13] & 0x07FF)>>1);
    packet[20] = (uint8_t) ((output[13] & 0x07FF)>>9 | (output[14] & 0x07FF)<<2);
    packet[21] = (uint8_t) ((output[14] & 0x07FF)>>6 | (output[15] & 0x07FF)<<5);
    packet[22] = (uint8_t) ((output[15] & 0x07FF)>>3);
    packet[23] = (uint8_t) stateByte; //Flags byte
    packet[24] = (uint8_t) SBUS_FRAME_FOOTER; //Footer
*/



#endif // RX module

#ifdef TX_module
    int _channels[18];
    int _failsafe;
    long _goodFrames;
    long _lostFrames;
    long _decoderErrorFrames;
    long long _lastGoodFrame;
    
boolean read_frame() {
  static byte buffer[SBUS_PACKET_LENGTH];
  static byte buffer_index = 0;
  while (Serial.available()) {
    byte rx = Serial.read();
    if (buffer_index == 0 && rx != SBUS_FRAME_HEADER) {
      //incorrect start byte, out of sync
      _decoderErrorFrames++;
      continue;      
    }
    buffer[buffer_index++] = rx;

    if (buffer_index == 25) {
      buffer_index = 0;
      if (buffer[24] != SBUS_FRAME_FOOTER) {
        //incorrect end byte, out of sync
        _decoderErrorFrames++;
        continue;
      }
      _goodFrames++;

      Servos[0]  = ((buffer[1]    |buffer[2]<<8)                 & 0x07FF);
      Servos[1]  = ((buffer[2]>>3 |buffer[3]<<5)                 & 0x07FF);
      Servos[2]  = ((buffer[3]>>6 |buffer[4]<<2 |buffer[5]<<10)  & 0x07FF);
      Servos[3]  = ((buffer[5]>>1 |buffer[6]<<7)                 & 0x07FF);
      Servos[4]  = ((buffer[6]>>4 |buffer[7]<<4)                 & 0x07FF);
      Servos[5]  = ((buffer[7]>>7 |buffer[8]<<1 |buffer[9]<<9)   & 0x07FF);
      Servos[6]  = ((buffer[9]>>2 |buffer[10]<<6)                & 0x07FF);
      Servos[7]  = ((buffer[10]>>5|buffer[11]<<3)                & 0x07FF);
      Servos[8]  = ((buffer[12]   |buffer[13]<<8)                & 0x07FF);
      Servos[9]  = ((buffer[13]>>3|buffer[14]<<5)                & 0x07FF);
      Servos[10] = ((buffer[14]>>6|buffer[15]<<2|buffer[16]<<10) & 0x07FF);
      Servos[11] = ((buffer[16]>>1|buffer[17]<<7)                & 0x07FF);
      Servos[12] = ((buffer[17]>>4|buffer[18]<<4)                & 0x07FF);
      Servos[13] = ((buffer[18]>>7|buffer[19]<<1|buffer[20]<<9)  & 0x07FF);
      Servos[14] = ((buffer[20]>>2|buffer[21]<<6)                & 0x07FF);
      Servos[15] = ((buffer[21]>>5|buffer[22]<<3)                & 0x07FF);

      ((buffer[23])      & 0x0001) ? Servos[16] = 2047: Servos[16] = 0;
      ((buffer[23] >> 1) & 0x0001) ? Servos[17] = 2047: Servos[17] = 0;

      if ((buffer[23] >> 3) & 0x0001) {
        failsafe_state = true;
      } else {
        failsafe_state = false;
      }

      if ((buffer[23] >> 2) & 0x0001) {
        _lostFrames++;
      }

      _lastGoodFrame = millis();

    }
    
  }
  
}

#endif // TX_module

#endif // IBUS_module
