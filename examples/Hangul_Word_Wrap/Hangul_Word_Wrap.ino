/*This example wraps libhangul and includes a function get_arduino_char() that switches 
 * the output of the preedit strings and commit strings produced theirin from unicode codepoints
 * to utf8 codepoints so that we can print the characters on arduino based devices.
 * this example prints to the serial information that the function would ultimately be passing to a text renderer:
 * 1: the characther to display
 * 2: whether a character is final (and thus the renderer should advance to a new cell)
 * 3: whether a space has been intercepted (and thust the renderer ought place a space)
 *
 *
 *Hangul text entry is more complicated than engilsh because the characters which 
 *comprise a sylable get stacked atop one another, rather than being strung along linearly.
 *So as you make keystrokes, you generate and display what is called a pre-edit string. 
 *From our example:
 *v corresponds to ㅍ
 *k corresponds to ㅏ
 *d correpsonds to ㅇ
 *but the letters stack, so typing vk produces 파 rather than ㅍㅏ.
 *Visually, it will display ㅍ and then replace that cell with 파 as more keys are entered.
 * 
 * typing vkd produces 팡, but the logic in libhangul does not stop there. as more keys are entered, 
 * libhangul decides whether characters are two or three jamo long based on a Lead, vowel, Tail pattern 
 * that exists in the korean language. 
 * so after the machine has seen enough of the pattern (in this case, four characters), as you enter keys vkdl
 * it will display ㅍ 파 팡 파이 in sequence. Once itknows to move on to a new key, it has a commit string and a new pre-edit string. 
 * 
 * This example includes the functions generateLocalBuffer(), test(), and returnHangulText(). 
 * Of these, generateLocalBuffer runs a loop on input text to convert the keys to the correct symbols, test() allows for inputting
 * test cases, and returnHangulText() wraps generateLocalBuffer to make it very easy to use (give returnHangulText a string of English
 * Keystrokes, and it will return the Hangul localization). 
*/

#include <hangul.h>
#include <inttypes.h>

String input1 = "vkdlTjs gksrmf fkdlqmfjfl xptmxm";//removed |fkdlqmfjfl xptmxm| from back
String correct1 = "파이썬 한글 라이브러리 테스트";//removed |라이브러리 테스트| from back

String input2 = "vkdl fkdlqmfjfl xptmxmv";
String correct2 = "파이 라이브러리 테스틒";

String input3 = "sudT";
String correct3 = "녕ㅆ";

String input4 = "s u d";
String correct4 = "ㄴ ㅕ ㅇ";

String inputBig = "vkdlTjs gksrmf fkdlqmfjfl xptmxm vkdlTjs gksrmf fkdlqmfjfl xptmxm vkdlTjs gksrmf fkdlqmfjfl xptmxm vkdlTjs gksrmf fkdlqmfjfl xptmxm vkdlTjs gksrmf fkdlqmfjfl xptmxm vkdlTjs gksrmf fkdlqmfjfl xptmxm vkdlTjs gksrmf fkdlqmfjfl xptmxm vkdlTjs gksrmf fkdlqmfjfl xptmxm vkdlTjs gksrmf fkdlqmfjfl xptmxm";
String correctBig = "파이썬 한글 라이브러리 테스트 파이썬 한글 라이브러리 테스트 파이썬 한글 라이브러리 테스트 파이썬 한글 라이브러리 테스트 파이썬 한글 라이브러리 테스트 파이썬 한글 라이브러리 테스트 파이썬 한글 라이브러리 테스트 파이썬 한글 라이브러리 테스트 파이썬 한글 라이브러리 테스트";


void setup() {
    Serial.begin(9600);
    int testnumber = 0;
}

static int ucscharlen(const ucschar *str)
{
    const ucschar *end = str;
    while (*end != 0)
  end++;
    return end - str;
}

String composeBuffer = "";
void composeBufferToDisplayLocalLang(){
  
    bool check_if_space;
    bool isfinal;
    const char* keyboard = "2";
    String currentWord= "";
    int8_t charWidth = 8;
    int maxCharPerLine = 30/int(charWidth);
    String currentLineBuffer = "";
    int wordSize = 0;
    int positionOnLine = 0;
    
    char output[32];
    HangulInputContext *hic = hangul_ic_new(keyboard); //Brandon says never optimize. This feels very unoptimal. 
    String result = "";

    int lastSpaceIndex = composeBuffer.lastIndexOf(' ');
    
    if(lastSpaceIndex > 1){//there are spaces, so we can primarily wrap on those. 
      for(int index=0; index <= composeBuffer.length(); index++)
      {
          //check if space outside of get_Arduino_char!!!!!!!!!
          check_if_space = handle_spaces(composeBuffer[index]);
          
          isfinal = get_arduino_char(hic,composeBuffer[index], output);
          if(isfinal==true)
          {
            currentWord += output;//in our case, result is rendering the final string as it is formed.
            wordSize += 1;
             
          }
          if(check_if_space == 1){ //
             if(wordSize+positionOnLine > maxCharPerLine){//if we exceed the symbol limit on 
              currentWord = "\n" + currentWord +" ";      //this line, we word wrap, appending
              Serial.print(currentWord);                  //a new line, the word, and a space. 
              result += currentWord;
              //bookeeping to reset the word and line sizes
              currentWord = "";
              positionOnLine = wordSize+1;
              wordSize = 0; 
            } else{ //add a space only if we don't word wrap. 
              currentWord += " ";
              wordSize += 1;
            }
           
          }
          //Serial.printf("the char is final: %d, there was a space %d, the char is:", isfinal, check_if_space);
          //Serial.println(output); //rendering characters as they ought be displayed in sequence with finality denoted. 
     
      }
    Serial.print(currentWord);
    //print any characters remaining after that last space. 
    } else if(lastSpaceIndex == -1){//there are no spaces, so we cannot depend on spaces to print.  
      for(int index = 0; index <= composeBuffer.length(); index++)
      {
        
        isfinal = get_arduino_char(hic,composeBuffer[index], output);
        Serial.printf("the char is final: %d, index is %d, the char is:", isfinal, index);
        Serial.println(output); //rendering characters as they ought be displayed in sequence with finality denoted. 
      }
    
//        if(isfinal==true)
//        {
//          currentWord += output;//in our case, result is rendering the final string as it is formed.
//          positionOnLine += 1;
//          if(positionOnLine >= maxCharPerLine){
//            currentWord = currentWord+'\n';
//            Serial.print(currentWord);
//            positionOnLine = 0;
//            result += currentWord;
//            currentWord = "";
//            
//          }
//          
//        }else{//is final is false, so check if we're at the end of our string. 
//          if(index = composeBuffer.length()-1){ //if the last character isn't a final character...run with it!
//            Serial.printf("buffer is %d long\n", composeBuffer.length());
//            Serial.printf("we are at the %dnth character of the word\n", index);
//            
//            currentWord += output;//
//            positionOnLine += 1;
//            if(positionOnLine >= maxCharPerLine){
//              currentWord = currentWord+'\n';
//              Serial.print(currentWord);
//              positionOnLine = 0;
//              result += currentWord;
//              currentWord = "";
//            }
//            Serial.print(currentWord);
//          }
//        }
//      }
//      Serial.print(currentWord);
    }else{//start at the last space, do the rest of the chars. 
      for(int index = lastSpaceIndex; index < composeBuffer.length(); index++){
      {
        isfinal = get_arduino_char(hic,composeBuffer[index], output);
        if(isfinal==true)
        {
          currentWord += output;//in our case, result is rendering the final string as it is formed.
          positionOnLine += 1;
          if(positionOnLine >= maxCharPerLine){
            currentWord = currentWord+'\n';
            Serial.print(currentWord);
            positionOnLine = 0;
            result += currentWord;
            currentWord = "";
          }
        }
      }
    } 
  }
}

void loop(){
  while(!Serial);
  int timestamp1;
  int timestamp2;

  Serial.println("++++++++++++++Test 1+++++++++++++++");
  Serial.print("output: ");
  composeBuffer = input1;
  composeBufferToDisplayLocalLang();
  Serial.print("\nun-wrapped text: ");
  Serial.println(correct1);
  
  Serial.println("++++++++++++++Test 2+++++++++++++++");
  Serial.print("output: ");
  composeBuffer = input2;
  composeBufferToDisplayLocalLang();
  Serial.print("\nun-wrapped text: ");
  Serial.println(correct2);

  Serial.println("++++++++++++++Test 3+++++++++++++++");
  Serial.print("output: ");
  composeBuffer = input3;
  composeBufferToDisplayLocalLang();
  Serial.print("\nun-wrapped text: ");
  Serial.println(correct3);

  Serial.println("++++++++++++++Test 4+++++++++++++++");
  Serial.print("output: ");
  composeBuffer = input4;
  composeBufferToDisplayLocalLang();
  Serial.print("\nun-wrapped text: ");
  Serial.println(correct4);

//  Serial.println("++++++++++++++Test big+++++++++++++++");
//  Serial.print("output: ");
//  composeBuffer = inputBig;
//  timestamp1 = millis();
//  composeBufferToDisplayLocalLang();
//  timestamp2 = millis();
//  Serial.print("\nun-wrapped text: ");
//  Serial.println(correctBig);
//  Serial.printf("big test duration: %d", timestamp2-timestamp1); 
  
  delay(10000);
  Serial.println();
}
