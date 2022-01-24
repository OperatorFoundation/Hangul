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

String keying0 = "f";
String kcorrect0 = "ㄹ";

String keying1 = "fk";
String kcorrect1 = "라";

String keying2 = "fkd";
String kcorrect2 = "랑";

String keying3 = "fkdl";
String kcorrect3 = "라이";

String keying4 = "fkdlq";
String kcorrect4 = "라입";

String keying5 = "fkdlqm";
String kcorrect5 = "라이브";

String keying6 = "fkdlqmf";
String kcorrect6 = "라이블";

String keying7 = "fkdlqmfj";
String kcorrect7 = "라이브럴";

String keying8 = "fkdlqmfjf";
String kcorrect8 = "라이브럴";

String keying9 = "fkdlqmfjfl";
String kcorrect9 = "라이브러리";

//String keying10 = "fkdlqmfjfl ";
//String kcorrect10 = "라이브러리 ";
//
//String keying11 = "fkdlqmfjfl g";
//String kcorrect11 = "라이브러리 ㅎ"
//
//String keying12 = "fkdlqmfjfl gk";
//String kcorrect12 = "라이브러리 하"
//
//String keying13 = "fkdlqmfjfl gks";
//String kcorrect13 = "라이브러리 한"
//
//String keying14 = "fkdlqmfjfl gksr";
//String kcorrect14 = "라이브러리 한ㄱ"
//
//String keying15 = "fkdlqmfjfl gksrm";
//String kcorrect15 = "라이브러리 한그"
//
//String keying16 = "fkdlqmfjfl gksrmf";
//String kcorrect16 = "라이브러리 한글"

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

  Serial.println("++++++++++++++Keying0+++++++++++++++");
  Serial.print("output: ");
  composeBuffer = keying0+ " ";
  composeBufferToDisplayLocalLang();
  Serial.print("\nun-wrapped text: ");
  Serial.println(kcorrect0);

  Serial.println("++++++++++++++Keying1+++++++++++++++");
  Serial.print("output: ");
  composeBuffer = keying1+ " ";
  composeBufferToDisplayLocalLang();
  Serial.print("\nun-wrapped text: ");
  Serial.println(kcorrect1);

  Serial.println("++++++++++++++Keying2+++++++++++++++");
  Serial.print("output: ");
  composeBuffer = keying2+ " ";
  composeBufferToDisplayLocalLang();
  Serial.print("\nun-wrapped text: ");
  Serial.println(kcorrect2);

  Serial.println("++++++++++++++Keying3+++++++++++++++");
  Serial.print("output: ");
  composeBuffer = keying3+ " ";
  composeBufferToDisplayLocalLang();
  Serial.print("\nun-wrapped text: ");
  Serial.println(kcorrect3);

  Serial.println("++++++++++++++Keying4+++++++++++++++");
  Serial.print("output: ");
  composeBuffer = keying4+ " ";
  composeBufferToDisplayLocalLang();
  Serial.print("\nun-wrapped text: ");
  Serial.println(kcorrect4);

  Serial.println("++++++++++++++Keying5+++++++++++++++");
  Serial.print("output: ");
  composeBuffer = keying5+ " ";
  composeBufferToDisplayLocalLang();
  Serial.print("\nun-wrapped text: ");
  Serial.println(kcorrect5);

  Serial.println("++++++++++++++Keying6+++++++++++++++");
  Serial.print("output: ");
  composeBuffer = keying6+ " ";
  composeBufferToDisplayLocalLang();
  Serial.print("\nun-wrapped text: ");
  Serial.println(kcorrect6);

  Serial.println("++++++++++++++Keying7+++++++++++++++");
  Serial.print("output: ");
  composeBuffer = keying7+ " ";
  composeBufferToDisplayLocalLang();
  Serial.print("\nun-wrapped text: ");
  Serial.println(kcorrect7);

  Serial.println("++++++++++++++Keying8+++++++++++++++");
  Serial.print("output: ");
  composeBuffer = keying8+ " ";
  composeBufferToDisplayLocalLang();
  Serial.print("\nun-wrapped text: ");
  Serial.println(kcorrect8);

  Serial.println("++++++++++++++Keying9+++++++++++++++");
  Serial.print("output: ");
  composeBuffer = keying9+ " ";
  composeBufferToDisplayLocalLang();
  Serial.print("\nun-wrapped text: ");
  Serial.println(kcorrect9);

  //for(int index = 0; index < 

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
