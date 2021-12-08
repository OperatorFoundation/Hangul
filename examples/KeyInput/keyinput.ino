// this is for the hangul wrapper
#include <hangul.h>
#include <cassert>
#include <inttypes.h>

const char* hangulkeyboard = "2";
HangulInputContext *hic = hangul_ic_new(hangulkeyboard);

String localBuffer = "";
String localBufferA = "";

void generateLocalBuffer(char keystroke, String output)
{
  char *arduinoChar;
  Serial.print(keystroke);
  bool isFinal = get_arduino_char(hic, keystroke, arduinoChar);
  Serial.print(arduinoChar);
  if (isFinal == true)
  {
    localBuffer += output;
//    strcat(localBuffer, output);
  }
  else
  {
    localBufferA = localBuffer;
    localBufferA += output;
//    strcat(localBufferA, output);
  }
}
  
void setup()   
{  
    Serial.begin(9600);  
}  
  
void loop()   
{
   while (!Serial);
   Serial.println("display buffer: ");
   if (Serial.available() == 0){//checks for user input
     Serial.flush();
     String textInputString = Serial.readString();
     Serial.print("textInputString: ");
     Serial.println(textInputString);
     char textInputChar[1];
     textInputString.toCharArray(textInputChar, 1);
     
     generateLocalBuffer(*textInputChar, localBuffer);
     Serial.println(localBuffer);
   }
} 
