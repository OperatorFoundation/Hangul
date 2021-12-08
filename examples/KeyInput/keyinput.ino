// this is for the hangul wrapper
#include <hangul.h>
#include <cassert>
#include <inttypes.h>



const char* hangulkeyboard = "2";
HangulInputContext *hic = hangul_ic_new(hangulkeyboard);

String localBuffer = "";
String localBufferA = "";

bool get_arduino_char1(HangulInputContext *hic, char keystroke, char *output)
/* Function get_arduino_char passes a keystroke through the hic, and converts the ucschar produced by
 * the hic into a char built from a uint8_t[4] filled by utf8 encoded values that are equivalent to the ucschar
 * it transforms that into a char and passes it back out, and returns whether or not the hic thinks this is a final character
 * ++++++++++
 * Parameters 
 * *hic, pointer to the HangulInputContext
 * keystroke, the keystroke we are converting to hangul
 * *output, pointer to the utf8 encoded hangul character the function produces. 
 * ++++++++++
 * RETURNS
 * True or False, True if the character is a final form of a hangul or False if a building block for temporary display. 
 * *output, passed in as an output parameter, now filled with a character. 
*/
{
  Serial.print("  getarduinoCharkeystroke: ");
  Serial.print(keystroke);
  int ret = hangul_ic_process(hic, keystroke);//handles backspaces and keyboards
  uint8_t utf8[4]={0,0,0,0};
  uint8_t pre_edit[4] = {0,0,0,0};
  char* single;
  int ascii;
  Serial.println("test1");

  const ucschar *commit_string = hangul_ic_get_commit_string(hic); //get the preedit strings by passing characters (simulating keystrokes into the get_string functions
  if (*commit_string == 0)
  {
    Serial.println("test2a");
    const ucschar *preedit_string = hangul_ic_get_preedit_string(hic);
    unsigned len2 = unicode_codepoint_to_utf8(pre_edit, *preedit_string);
    
    
    single = ((char *)pre_edit);
    Serial.println("test3");
    Serial.print("single: ");
    Serial.println(*single);
    memcpy(output, single, 32);
    
    Serial.println("test4");
    
    Serial.print(*output);
    return false;
    }
  else
  {
    Serial.println("test2b");
    unsigned len = unicode_codepoint_to_utf8(utf8, *commit_string);//convert the uschars to utf8 ecoded uint8_t[4] arrays. 
    single = (char *)utf8;
    
       
    memcpy(output, single, 32);
    Serial.print(*output);
    return true;
    
    }

    hangul_ic_reset(hic);
    hangul_ic_delete(hic);
 }


void generateLocalBuffer(char keystroke, String output)
{
  char *arduinoChar;
  Serial.print("keystroke in generateLocalBuffer: ");
  Serial.println(keystroke);
  bool isFinal = get_arduino_char1(hic, keystroke, arduinoChar);
  Serial.print(isFinal);
  Serial.println("arduinoHangul: ");
  Serial.println(arduinoChar);
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
   if (Serial.available() > 0){//checks for user input
     String textInputString = Serial.readString();
     Serial.print("textInputString: ");
     Serial.println(textInputString);
     char textInputChar = textInputString.charAt(0);
     Serial.print("textInputChar: ");
     Serial.println(textInputChar);
     generateLocalBuffer(textInputChar, localBuffer);
     Serial.println(localBuffer);
   }
} 
