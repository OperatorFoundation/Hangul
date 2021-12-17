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
 * it will display ㅍ 파 팡 파이 in sequence. Once it knows to move on to a new cell, it has a commit string and pushes that as a final character, and moves on to constructing a new pre-edit string. 
 * by following that process, it builds characters in 1s, 2s, or 3s, based on the pattern. 
 *
 * this example includes composeBufferToDisplayLocalLang(), a function that calls wrapOnSpaces and wrapNoSpaces to handle word-wrapping on a global variable composeBuffer. But the execution loop cheats by adding a trailing space to every string, rather than including any handling for the last word in the segment. 
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

String inputBig = "vkdlTjsgksrmffkdlqmfjflxptmxmvkdlTjsgksrmf fkdlqmfjfl xptmxm vkdlTjs gksrmf fkdlqmfjfl xptmxm vkdlTjs gksrmf fkdlqmfjfl xptmxm vkdlTjs gksrmf fkdlqmfjfl xptmxm vkdlTjs gksrmf fkdlqmfjfl xptmxm vkdlTjs gksrmf fkdlqmfjfl xptmxm vkdlTjs gksrmf fkdlqmfjfl xptmxm vkdlTjs gksrmf fkdlqmfjfl xptmxm";
String correctBig = "파이썬한글라이브러리테스트파이썬한글라이브러리 테스트 파이썬 한글 라이브러리 테스트 파이썬 한글 라이브러리 테스트 파이썬 한글 라이브러리 테스트 파이썬 한글 라이브러리 테스트 파이썬 한글 라이브러리 테스트 파이썬 한글 라이브러리 테스트 파이썬 한글 라이브러리 테스트";

String composeBuffer = "";

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

void wrapNoSpaces(int indexStart){
    bool isfinal;
    const char* keyboard = "2";
    String currentWord= "";
    int8_t charWidth = 8;
    int maxCharPerLine = 20/int(charWidth);
    String currentLineBuffer = "";
    int wordSize = 0;
    int positionOnLine = 0;
    
    char output[32];
    HangulInputContext *hic = hangul_ic_new(keyboard); //Brandon says never optimize. This feels very unoptimal. 
    String result = "";
    
    for(int index=indexStart; index <= composeBuffer.length(); index++){     
      isfinal = get_arduino_char(hic,composeBuffer[index], output);
      if(isfinal==true){
        currentWord += output;//in our case, result is rendering the final string as it is formed.
        wordSize += 1;
      }
      if(wordSize+positionOnLine > maxCharPerLine){//if we exceed the symbol limit on 
        currentWord = "\n" + currentWord +" ";      //this line, we word wrap, appending
        Serial.print(currentWord);                  //a new line, the word, and a space. 
        result += currentWord;
        //bookeeping to reset the word and line sizes
        currentWord = "";
        positionOnLine = wordSize+1;
        wordSize = 0; 
        }    
      //Serial.printf("the char is final: %d, there was a space %d, the char is:", isfinal, check_if_space);
      //Serial.println(output); //rendering characters as they ought be displayed in sequence with finality denoted. 
  }
  Serial.print(currentWord);//print any characters remaining after that last space. 
}

void wrapOnSpaces(){
        bool check_if_space;
        bool isfinal;
            const char* keyboard = "2";
    String currentWord= "";
    int8_t charWidth = 8;
    int maxCharPerLine = 100/int(charWidth);
    Serial.printf("max chars: %d \n",maxCharPerLine);
    String currentLineBuffer = "";
    int wordSize = 0;
    int positionOnLine = 0;
    
    char output[32];
    HangulInputContext *hic = hangul_ic_new(keyboard);  
    String result = "";
        for(int index=0; index <= composeBuffer.length(); index++)
        {
        if(wordSize == maxCharPerLine){
          Serial.println(currentLineBuffer);//first, dump the line)
          currentLineBuffer = currentWord; //then make the line the too-long word
          Serial.println(currentLineBuffer);
          currentLineBuffer="";
          currentWord = "";
          positionOnLine = 0;
          wordSize = 0;
        }
        check_if_space = handle_spaces(composeBuffer[index]);
        
        isfinal = get_arduino_char(hic,composeBuffer[index], output);
        if(isfinal==true)
        {
          currentWord += output;//
          wordSize += 1;
          if(wordSize> maxCharPerLine){ //we must segment the word
              //currentWord += "\n";
              currentLineBuffer+=currentWord;
              currentWord = "";
              positionOnLine = wordSize;
              wordSize = 0;
          }       
           
        }
        if(check_if_space == 1){ //
          //building a word and we found a space. can either append to the line or start a new line. 
                 
           if(wordSize+positionOnLine > maxCharPerLine){//our word doesn't fit, so start a new line
            Serial.println(currentLineBuffer); //dump the buffer
            currentLineBuffer = currentWord+" "; //empty the buffer and fill our new word
            positionOnLine = wordSize+1;
            currentWord = "";//empty the word
            wordSize = 0;
            
           } else { //we add the word to the line. 
            currentLineBuffer += currentWord + " ";
            currentWord = "";
            positionOnLine += wordSize+1;
            wordSize = 0;
          }
        } 
    }
    Serial.println(currentLineBuffer);//print any characters remaining after that last space. 
  }



void composeBufferToDisplayLocalLang(){
    int lastSpaceIndex = composeBuffer.lastIndexOf(' ');
    if(lastSpaceIndex > 0){//there are spaces, so we can primarily wrap on those. 
      wrapOnSpaces();
      //wrapNoSpaces(lastSpaceIndex);//to print the last word
    } else if(lastSpaceIndex == -1){//there are no spaces, so we cannot depend on spaces to print.  
      wrapNoSpaces(0);
    }   
}

void loop(){
  while(!Serial);
  int timestamp1;
  int timestamp2;

  Serial.println("++++++++++++++Test 1+++++++++++++++");
  Serial.print("output: ");
  composeBuffer = input1+ " ";
  composeBufferToDisplayLocalLang();
  Serial.print("\nun-wrapped text: ");
  Serial.println(correct1);
  
  Serial.println("++++++++++++++Test 2+++++++++++++++");
  Serial.print("output: ");
  composeBuffer = input2+ " ";
  composeBufferToDisplayLocalLang();
  Serial.print("\nun-wrapped text: ");
  Serial.println(correct2);

  Serial.println("++++++++++++++Test 3+++++++++++++++");
  Serial.print("output: ");
  composeBuffer = input3+ " ";
  composeBufferToDisplayLocalLang();
  Serial.print("\nun-wrapped text: ");
  Serial.println(correct3);

  Serial.println("++++++++++++++Test 4+++++++++++++++");
  Serial.print("output: ");
  composeBuffer = input4+ " ";
  composeBufferToDisplayLocalLang();
  Serial.print("\nun-wrapped text: ");
  Serial.println(correct4);

//  Serial.println("++++++++++++++Test big+++++++++++++++");
//  Serial.print("output: ");
//  composeBuffer = inputBig+ " ";
//  timestamp1 = millis();
//  composeBufferToDisplayLocalLang();
//  timestamp2 = millis();
//  Serial.print("\nun-wrapped text: ");
//  Serial.println(correctBig);
//  Serial.printf("big test duration: %d", timestamp2-timestamp1); 
//  
  delay(10000);
  Serial.println();
}
