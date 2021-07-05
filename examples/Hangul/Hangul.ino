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
 * In this example, we use isfinal to pass out the statement that libhangul believes a character is final. 
*/

#include <hangul.h>
#include <inttypes.h>

char *input = "vkdlTjs gksrmf fkdlqmfjfl xptmxm";//removed |fkdlqmfjfl xptmxm| from back
char *correct = "파이썬 한글 라이브러리 테스트";//removed |라이브러리 테스트| from back


void setup() {
    Serial.begin(9600);
}

void loop() 
{
  bool check_if_space;
  bool isfinal;
  const char* keyboard = "2";
  
  char output[32];
  HangulInputContext *hic = hangul_ic_new(keyboard); //Brandon says never optimize. This feels very unoptimal. 
  String result = "";
  
  for(int index=0; index <= strlen(input); index++)
  {
      //check if space outside of get_Arduino_char!!!!!!!!!
      check_if_space = handle_spaces(input[index]);
      
      isfinal = get_arduino_char(hic,input[index], output);
      if(isfinal==true)
      {
        result = result + output;//in our case, result is rendering the final string as it is formed. 
      }
      if(check_if_space == 1){
        result = result+' ';
      }
      Serial.printf("the char is final: %d, there was a space %d, the char is:", isfinal, check_if_space);
      Serial.println(output); //rendering characters as they ought be displayed in sequence with finality denoted. 
  }
    

    Serial.print("Input:   ");
    Serial.println(input);
    Serial.print("Result:  ");
    Serial.println(result);
    Serial.print("Correct: ");
    Serial.println(correct);

    delay(3000);

} 
