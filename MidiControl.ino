// Name: MidiControl3
// Author: Irza Pulungan
// Date: Mar 2014
// Version: 0.2
// Latest status: Working with DFU - Arduino MIDI
// Purpose: Interface my own midi controller
// Notes:

#include <Bounce.h>



#define debug 0
#define MIDI_COMMAND_NOTE_OFF       0x80   // 10000000
#define MIDI_COMMAND_NOTE_ON        0x90   // 10010000
#define MIDI_COMMAND_CC_CHANGE      0xB0   // 10110000

/* The format of the message to send via serial */
typedef union {
    struct {
	uint8_t command;
	uint8_t channel;
	uint8_t data2;
	uint8_t data3;
    } msg;
    uint8_t raw[4];
} t_midiMsg;

//add one to num
#define NUM_AI 6
#define NUM_DI 13
// anaval adalah tempat penyimpanan value sementara untuk di compare
// digival adalah tempat penyimapanan value digital
int anapin[]={A0,A1,A2,A3,A4,A5};
int anaval[]={0,0,0,0,0,0};
int digipin[] = {12,13,2,3,4,5,6,7,8,9,10,11};
int digival[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int analogInput;
int i = 0;

// tulis disini semua isi digipin
// berguna untuk menulis bouncing
Bounce bouncing[] = {Bounce(12, 5), Bounce(13, 5), Bounce(2, 5),Bounce(3, 5), Bounce(4, 5), Bounce(5, 5), Bounce(6, 5), Bounce(7, 5), Bounce(8, 5), Bounce(9, 5), Bounce(10, 5), Bounce(11, 5)};
void setup() 
{
  Serial.begin(115200);
  // Default speed of the Serial to MIDI Converter serial port??
  delay(200);

  for (i = 0; i < NUM_AI; i++){
    pinMode(digipin[i],INPUT);
  }
  
  
}

void loop() 
  {
    t_midiMsg midiMsg;
    for (i = 0; i < NUM_AI; i++){
      analogInput = map(analogRead(anapin[i]),1,1023,0,127);
      delay(5);
      if (analogInput < anaval[i]-1 || analogInput > anaval[i]+1) {  
        if (debug == 1) {
          Serial.print("pin ");
          Serial.print(i);
          Serial.print(":");
          Serial.println(analogInput);
       } else {
         int CONT = 17 + i;
         // set yang mau di kirim
       	 midiMsg.msg.command = MIDI_COMMAND_CC_CHANGE;
       	 midiMsg.msg.channel = 1;
         midiMsg.msg.data2   = CONT;
         midiMsg.msg.data3   = analogInput;	/* Velocity */
       
	 /* Send note on */
         Serial.write(midiMsg.raw, sizeof(midiMsg));
         delay(5);
        }
      
      }
         anaval[i] = analogInput;
    }
  
        bounceupdate();
        //delay(1);
  }


void bounceupdate()
  {
    t_midiMsg midiMsg;
    for (i=0; i<NUM_DI ; i++) 
    {
      bouncing[i].update();
      int value = bouncing[i].read();
      if (value != digival[i]) {
         if (value == HIGH) {
             if (debug == 1) {
                Serial.print("button ");
                Serial.print(i);
                Serial.println("DOWN");
             } else {
                // berharap jatuh di C2
                int note = 36 + i;
                //MIDI.sendNoteOn(note,127,MIDI_OUT_CHAN);
         	midiMsg.msg.command = MIDI_COMMAND_NOTE_ON;
        	midiMsg.msg.channel = 1;
	        midiMsg.msg.data2   = note;
	        midiMsg.msg.data3   = 64;	/* Velocity */

	        /* Send note on */
	        Serial.write(midiMsg.raw, sizeof(midiMsg));
	        delay(5);
             }
         
         } else {
              if (debug == 1) {
                Serial.print("button ");
                Serial.print(i);
                Serial.println("UP");
              } else {
                int note = 36 + i;
                //MIDI.sendNoteOff(note,0,MIDI_OUT_CHAN);
                midiMsg.msg.command = MIDI_COMMAND_NOTE_OFF;
       	        midiMsg.msg.channel = 1;
	        midiMsg.msg.data2   = note;
	        midiMsg.msg.data3   = 64;	/* Velocity */

	        Serial.write(midiMsg.raw, sizeof(midiMsg));
                delay(5);
              }
         
         }
                digival[i] = value;
       }
     }
    }

