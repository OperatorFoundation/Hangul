// this is for the hangul wrapper
#include <hangul.h>
#include <cassert>
#include <inttypes.h>



const char* hangulkeyboard = "2";
HangulInputContext *hic = hangul_ic_new(hangulkeyboard);


//Global Variables localBuffer and localBufferA are used by the generateLocalBuffer file to remember 
//all the text not held in 
char localBuffer[320];
char localBufferA[320];
int messageLength = 0;

void generateLocalBuffer(char keystroke, char *output, int messageLength){
  char* single;
  char arduinoChar[32];
  
  bool isFinal = get_arduino_char(hic, keystroke, arduinoChar);
  single = ((char *)arduinoChar);
  if (isFinal == true){    
    memcpy(localBuffer+messageLength*32, single, 32);
    Serial.print("localBuffer: ");
    Serial.println(localBuffer);
    Serial.printf("numchars: %d, adding at position %d, final", messageLength, messageLength+1);
    //fill output with localBuffer
    messageLength += 1; //must be at bottom. 
    Serial.printf("numchars: %d \n", messageLength);
  } else{
    memcpy(localBufferA, localBuffer, 320); //reset base of temporarybuffer. 
    Serial.printf("numchars: %d, adding at position %d, not final", messageLength, messageLength+1);
    memcpy(localBufferA+messageLength*32, single, 32);
    Serial.print("localBufferA: ");
    Serial.println(localBufferA);
    
    //fill output with localBufferA
  }
}



  
void setup()   
{  
    Serial.begin(9600);  
}  
  
void loop()   
{
   char composeCharBuffer[320];
   int bufferLength = 0;
   String composeBuffer;
   if (Serial.available() > 0){//checks for user input
     String textInputString = Serial.readString();
     
     char textInputChar = textInputString.charAt(0);
     generateLocalBuffer(textInputChar, composeCharBuffer,messageLength);
     Serial.println(composeBuffer);
   }
} 
