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

String input2 = "vkdl";
String correct2 = "파이";

String input3 = "sudT";
String correct3 = "녕ㅆ";

String inputBig = "vkdlTjs gksrmf fkdlqmfjfl xptmxm vkdlTjs gksrmf fkdlqmfjfl xptmxm vkdlTjs gksrmf fkdlqmfjfl xptmxm vkdlTjs gksrmf fkdlqmfjfl xptmxm vkdlTjs gksrmf fkdlqmfjfl xptmxm vkdlTjs gksrmf fkdlqmfjfl xptmxm vkdlTjs gksrmf fkdlqmfjfl xptmxm vkdlTjs gksrmf fkdlqmfjfl xptmxm vkdlTjs gksrmf fkdlqmfjfl xptmxm";
String correctBig = "파이썬 한글 라이브러리 테스트 파이썬 한글 라이브러리 테스트 파이썬 한글 라이브러리 테스트 파이썬 한글 라이브러리 테스트 파이썬 한글 라이브러리 테스트 파이썬 한글 라이브러리 테스트 파이썬 한글 라이브러리 테스트 파이썬 한글 라이브러리 테스트 파이썬 한글 라이브러리 테스트";


void setup() {
    Serial.begin(9600);
    int testnumber = 0;
}

//generateLocalBuffer handles feeding induvidual keystrokes to the libhangul library, and getting hangul out. 
String generateLocalBuffer(char keystroke, String result, HangulInputContext *hic){
      char arduinoChar[32];
      bool check_if_space;
      bool isfinal;
        //check if space outside of get_Arduino_char!!!!!!!!!
      check_if_space = handle_spaces(keystroke);
      
      isfinal = get_arduino_char(hic,keystroke, arduinoChar);
      if(isfinal==true)
      {
        result = result + arduinoChar;//in our case, result is rendering the final string as it is formed. 
      }
      if(check_if_space == 1){
        result = result+' ';
      }
            //Serial.(output); //rendering characters as they ought be displayed in sequence with finality denoted. 
      return result;
}

//test returns true or false depending on whether the input, once localized to hangul, matches the correct string,
//to verify situations and edge cases. 
bool test(String input, String correct) 
{
  
  const char* keyboard = "2";
  HangulInputContext *hic = hangul_ic_new(keyboard); //Brandon says never optimize. This feels very unoptimal. 
  
  String result = "";
  
  for(int index=0; index <= (input.length()); index++)
  {
 
    result = generateLocalBuffer1(input.charAt(index), result, hic);  
  }
    

    Serial.print("Input:   ");
    Serial.println(input);
    Serial.print("Result:  ");
    Serial.println(result);
    Serial.print("Correct: ");
    Serial.println(correct);
    if(result.equals(correct)){
      return true;
      
    }else{
      return false;
    }

    delay(3000);

}

//returnHangul text runs a loop across a string of english characters to turn them into a string of localized Hangul characters. 
String returnHangultext(String englishText){
  const char* keyboard = "2";
  HangulInputContext *hic = hangul_ic_new(keyboard); //Brandon says never optimize. This feels very unoptimal. 
  
  String result = "";
  
  for(int index=0; index <= (englishText.length()); index++)
  {
 
    result = generateLocalBuffer(englishText.charAt(index), result, hic);  
  }
  return result;
}

void loop(){
  int timestamp1;
  int timestamp2;
  bool success = test(input1, correct1);
  Serial.printf("test %d success: %d \n",1, success);
  success = test(input2, correct2);
  Serial.printf("test %d success: %d \n",2, success);
  success = test(input3, correct3);
  Serial.printf("test %d success: %d \n",3, success);
  timestamp1 = millis();
  success = test(inputBig, correctBig);
  Serial.printf(" big test, succcess: %d \n", success);
  timestamp2 = millis();
 
  Serial.print(timestamp2-timestamp1);
  delay(10000);
}

