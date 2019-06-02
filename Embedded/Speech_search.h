#ifndef Speech_search_H
#define Speech_search_H
#include "Arduino.h"
#include <WiFiClientSecure.h>
#include "Requests.h"

const int SAMPLE_FREQ = 8000;                          // Hz, telephone sample rate
const int SAMPLE_DURATION = 5.5;                        // duration of fixed sampling (seconds)
const int NUM_SAMPLES = SAMPLE_FREQ * SAMPLE_DURATION;  // number of of samples
const int ENC_LEN = (NUM_SAMPLES + 2 - ((NUM_SAMPLES + 2) % 3)) / 3 * 4;  // Encoded length of clip
char speech_data[ENC_LEN + 200] = {0}; //global used for collecting speech data

const char PREFIX[] = "{\"config\":{\"encoding\":\"MULAW\",\"sampleRateHertz\":8000,\"languageCode\": \"en-US\"}, \"audio\": {\"content\":\"";
const char SUFFIX[] = "\"}}"; //suffix to POST request
const int AUDIO_IN = A0; //pin where microphone is connected
const char API_KEY[] = ""; //key for Google Speech API

//WiFiClientSecure client; //global WiFiClient Secure object

const char PROGMEM b64_alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                    "abcdefghijklmnopqrstuvwxyz"
                                    "0123456789+/";
/* 'Private' declarations */
inline void a3_to_a4(unsigned char * a4, unsigned char * a3);
inline void a4_to_a3(unsigned char * a3, unsigned char * a4);
inline unsigned char b64_lookup(char c);

inline void a3_to_a4(unsigned char * a4, unsigned char * a3) {
  a4[0] = (a3[0] & 0xfc) >> 2;
  a4[1] = ((a3[0] & 0x03) << 4) + ((a3[1] & 0xf0) >> 4);
  a4[2] = ((a3[1] & 0x0f) << 2) + ((a3[2] & 0xc0) >> 6);
  a4[3] = (a3[2] & 0x3f);
}

inline void a4_to_a3(unsigned char * a3, unsigned char * a4) {
  a3[0] = (a4[0] << 2) + ((a4[1] & 0x30) >> 4);
  a3[1] = ((a4[1] & 0xf) << 4) + ((a4[2] & 0x3c) >> 2);
  a3[2] = ((a4[2] & 0x3) << 6) + a4[3];
}

inline unsigned char b64_lookup(char c) {
  if (c >= 'A' && c <= 'Z') return c - 'A';
  if (c >= 'a' && c <= 'z') return c - 71;
  if (c >= '0' && c <= '9') return c + 4;
  if (c == '+') return 62;
  if (c == '/') return 63;
  return -1;
}


int base64_encode(char *output, char *input, int inputLen) {
  int i = 0, j = 0;
  int encLen = 0;
  unsigned char a3[3];
  unsigned char a4[4];

  while (inputLen--) {
    a3[i++] = *(input++);
    if (i == 3) {
      a3_to_a4(a4, a3);

      for (i = 0; i < 4; i++) {
        output[encLen++] = pgm_read_byte(&b64_alphabet[a4[i]]);
      }

      i = 0;
    }
  }

  if (i) {
    for (j = i; j < 3; j++) {
      a3[j] = '\0';
    }

    a3_to_a4(a4, a3);

    for (j = 0; j < i + 1; j++) {
      output[encLen++] = pgm_read_byte(&b64_alphabet[a4[j]]);
    }

    while ((i++ < 3)) {
      output[encLen++] = '=';
    }
  }
  //  output[encLen] = '\0';
  return encLen;
}

int base64_decode(char * output, char * input, int inputLen) {
  int i = 0, j = 0;
  int decLen = 0;
  unsigned char a3[3];
  unsigned char a4[4];


  while (inputLen--) {
    if (*input == '=') {
      break;
    }

    a4[i++] = *(input++);
    if (i == 4) {
      for (i = 0; i < 4; i++) {
        a4[i] = b64_lookup(a4[i]);
      }

      a4_to_a3(a3, a4);

      for (i = 0; i < 3; i++) {
        output[decLen++] = a3[i];
      }
      i = 0;
    }
  }

  if (i) {
    for (j = i; j < 4; j++) {
      a4[j] = '\0';
    }

    for (j = 0; j < 4; j++) {
      a4[j] = b64_lookup(a4[j]);
    }

    a4_to_a3(a3, a4);

    for (j = 0; j < i - 1; j++) {
      output[decLen++] = a3[j];
    }
  }
  output[decLen] = '\0';
  return decLen;
}

int base64_enc_len(int plainLen) {
  int n = plainLen;
  return (n + 2 - ((n + 2) % 3)) / 3 * 4;
}

int base64_dec_len(char * input, int inputLen) {
  int i = 0;
  int numEq = 0;
  for (i = inputLen - 1; input[i] == '='; i--) {
    numEq++;
  }

  return ((6 * inputLen) / 8) - numEq;
}



int8_t mulaw_encode(int16_t sample) {
  const uint16_t MULAW_MAX = 0x1FFF;
  const uint16_t MULAW_BIAS = 33;
  uint16_t mask = 0x1000;
  uint8_t sign = 0;
  uint8_t position = 12;
  uint8_t lsb = 0;
  if (sample < 0)
  {
    sample = -sample;
    sign = 0x80;
  }
  sample += MULAW_BIAS;
  if (sample > MULAW_MAX)
  {
    sample = MULAW_MAX;
  }
  for (; ((sample & mask) != mask && position >= 5); mask >>= 1, position--)
    ;
  lsb = (sample >> (position - 4)) & 0x0f;
  return (~(sign | ((position - 5) << 4) | lsb));
}

class Wiki_search {
  public:
    bool last_wifi_state=true;
    const uint16_t IN_BUFFER_SIZE = 3000; //size of buffer to hold HTTP request
    const uint16_t OUT_BUFFER_SIZE = 1000; //size of buffer to hold HTTP response
    char request_buffer[3000]; //char array buffer to hold HTTP request
    char response_buffer[1000]; //char array buffer to hold HTTP post response
    char response[1000];
    int pin;
    uint32_t time_since_sample;      // used for microsecond timing
    const char*  SERVER = "speech.google.com";  // Server URL
    const int DELAY = 1000;
    const uint16_t RESPONSE_TIMEOUT = 6000;
    uint8_t old_button_state; //used for detecting button edges
    TFT_eSPI* tft;

    Wiki_search (TFT_eSPI* tft_to_use, int pin_to_use) {
      tft = tft_to_use;
      pin=pin_to_use;

    }

   void start(){
      last_wifi_state=true;
      tft->setCursor(0, 20, 1);
      tft->println("Press the upper button and say what you want to learn about");
    }


    //function used to record audio at sample rate for a fixed nmber of samples
    void record_audio() {
      int sample_num = 0;    // counter for samples
      int enc_index = strlen(PREFIX) - 1;  // index counter for encoded samples
      float time_between_samples = 1000000 / SAMPLE_FREQ;
      int value = 0;
      char raw_samples[3];   // 8-bit raw sample data array
      memset(speech_data, 0, sizeof(speech_data));
      sprintf(speech_data, "%s", PREFIX);
      char holder[5] = {0};
      Serial.println("starting");
      uint32_t text_index = enc_index;
      uint32_t start = millis();
      time_since_sample = micros();
      while (sample_num < NUM_SAMPLES && !digitalRead(pin)) { //read in NUM_SAMPLES worth of audio data
        value = analogRead(AUDIO_IN);  //make measurement
        raw_samples[sample_num % 3] = mulaw_encode(value - 1241); //remove 1.0V offset (from 12 bit reading)
        sample_num++;
        if (sample_num % 3 == 0) {
          base64_encode(holder, raw_samples, 3);
          strncat(speech_data + text_index, holder, 4);
          text_index += 4;
        }
        // wait till next time to read
        while (micros() - time_since_sample <= time_between_samples); //wait...
        time_since_sample = micros();
      }
      Serial.println(millis() - start);
      sprintf(speech_data + strlen(speech_data), "%s", SUFFIX);
      Serial.println("out");
    }



    void get_request(char * transcript) { //request to 608dev
      sprintf(request_buffer, "GET http://608dev.net/sandbox/sc/asselism/api_test.py?topic=%s&len=350 HTTP/1.1\r\n", transcript);
      strcat(request_buffer, "Host: 608dev.net\r\n"); //add more to the end
      strcat(request_buffer, "\r\n"); //add blank line!

      do_http_request("608dev.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
      Serial.println(response_buffer);
    }

    void update(uint8_t button_state, bool is_wifi_connected ) {
      if(!is_wifi_connected&&last_wifi_state)
      {
        tft->fillRect(0,20,130,140,TFT_BLACK);
        tft->setCursor(0, 20, 1);
        tft->println("This app requires wifi connection!");
      }
      else if (!button_state && button_state != old_button_state && is_wifi_connected) {
        tft->fillRect(0,20,130,140,TFT_BLACK);
        tft->setCursor(0, 20, 1);
        tft->println("Listening");
        Serial.println("listening...");
        record_audio();
        tft->fillRect(0,20,130,140,TFT_BLACK);
        tft->setCursor(0, 20, 1);
        tft->println("Processing");
        Serial.println("sending...");
        Serial.print("\nStarting connection to server...");
        delay(300);
        WiFiClientSecure client;

        if (!client.connect(SERVER, 443)) {
          Serial.println("Connection failed in search app!");
          tft->fillRect(0,20,130,140,TFT_BLACK);
          tft->setCursor(0, 20, 1);
          tft->println("Fail to connect the server! Please check wifi connection.");
          client.stop();
          return;
        } 
        else {
          Serial.println("Connected to server!");
          Serial.println(client.connected());
          int len = strlen(speech_data);
          // Make a HTTP request:
          client.print("POST /v1/speech:recognize?key="); client.print(API_KEY); client.print(" HTTP/1.1\r\n");
          client.print("Host: speech.googleapis.com\r\n");
          client.print("Content-Type: application/json\r\n");
          client.print("cache-control: no-cache\r\n");
          client.print("Content-Length: "); client.print(len);
          client.print("\r\n\r\n");
          int ind = 0;
          int jump_size = 1000;
          char temp_holder[jump_size + 10] = {0};
          Serial.println("sending data");
          while (ind < len) {
            delay(80);//experiment with this number!
            //if (ind + jump_size < len) client.print(speech_data.substring(ind, ind + jump_size));
            strncat(temp_holder, speech_data + ind, jump_size);
            client.print(temp_holder);
            ind += jump_size;
            memset(temp_holder, 0, sizeof(temp_holder));
          }
          client.print("\r\n");
          //Serial.print("\r\n\r\n");
          Serial.println("Through send...");
          unsigned long count = millis();
          while (client.connected()) {
            Serial.println("IN!");
            String line = client.readStringUntil('\n');
            Serial.print(line);
            if (line == "\r") { //got header of response
              Serial.println("headers received");
              break;
            }
            if (millis() - count > RESPONSE_TIMEOUT) break;
          }
          Serial.println("");
          Serial.println("Response...");
          count = millis();
          while (!client.available()) {
            delay(100);
            Serial.print(".");
            if (millis() - count > RESPONSE_TIMEOUT) break;
          }
          Serial.println();
          Serial.println("-----------");
          memset(response, 0, sizeof(response));
          while (client.available()) {
            char_append(response, client.read(), OUT_BUFFER_SIZE);
          }
          Serial.println(response);
          char* trans_id = strstr(response, "transcript");
          if (trans_id != NULL) {
            char* foll_coll = strstr(trans_id, ":");
            char* starto = foll_coll + 2; //starting index
            char* endo = strstr(starto + 1, "\""); //ending index
            int transcript_len = endo - starto + 1;
            char transcript[100] = {0};
            strncat(transcript, starto, transcript_len);
            Serial.println(transcript);
            get_request(transcript);

            if (response_buffer[0] == '-' && response_buffer[1] == '1') {
              tft->fillRect(0,20,130,140,TFT_BLACK);
              tft->setCursor(0, 20, 1);
              tft->println("I could not find anything on the topic of");
              tft->println(transcript);
              tft->println("");
              tft->println("Please try again.");
              tft->println("");
              tft->println("");
              tft->println("Press the upper button and say what you want to learn about");
            }
            else {
              tft->fillRect(0,20,130,140,TFT_BLACK);
              tft->setCursor(0, 20, 1);
              tft->println(response_buffer);
              tft->println("Press to search again");
            }
          }

          else {
            tft->fillRect(0,20,130,140,TFT_BLACK);
            tft->setCursor(0, 20, 1);
            tft->println("I did not hear this.");
            tft->println("");
            tft->println("Please try again.");
            tft->println("");
            tft->println("");
            tft->println("Press the upper button and say what you want to learn about");
          }
          Serial.println("-----------");
          client.stop();
          Serial.println("done");
        }
      }

      old_button_state = button_state;
      last_wifi_state=is_wifi_connected;
    }
};

#endif
