#include <hangul.h>
#include <inttypes.h>

char *input = "vkdlTjs gksrmf ";//removed |fkdlqmfjfl xptmxm| from back
char *correct = "파이썬 한글 ";//removed |라이브러리 테스트| from back

unsigned Unicode_CodepointToUTF8(uint8_t *utf8, uint32_t codepoint) {
  /*Function Unicode_CodepointToUTF8()
   * a function which converts from an int unicode codepoint to a utf8 int. 
   * for each number of used bits, 0-4, it does bitwise math to get just the relevant parts of the codepoint 
   *    
   * ++++++++++
   * Parameters
   * *utf8 : a pointer to the utf variable that this function fills with the properly formatted output. 
   * codepoint : a decimal representation of a unicode codepoint. U+AC00 would take 44032 or 0xAC00
   * ++++++++++
   * Returns
   * len (int): the number of BYTES used by the utf8 encoding NOTE: 4 BYTES WILL BE USED, 0 FILLED WHEN NOT USED BY UTF8 ENCODING. 
   * utf8(uint8_t): the function fills utf8 with the hex utf-8 ecoding that coordinates to the apropriate Unicode Datapoint.
   * ++++++++++
   * Notes: 
   * 0xc0 is is the leading code for any two octet block, 
   * 0xe0 is the leading code denoting a three octet block,  
   * 0xf0 is the leading code denoting a four octet block  
   * 0x80 is is the leading code for any subsequent of multi-octet blocks 
   */
  int secondplacedigit = 6; //slides the bits left, chucking away the bottom digit. 
  int thirdplacedigit = 12; //slides the bits left, chucking away the bottom 2 digits 
  int fourthplacedigit = 18; //slides the bits left, chucking away the bottom 3 digits
  int mask = 0x3F; //binary 111111, returns 1 for any 1 in the first 6 ranks, but 0 for everything after. (because 111111 is equal to 0000111111, so any digits bigger than it get eaten by its implied 0s)
  
  if (codepoint <= 0x7F) {//if the codepoint is smaller than 128, it onl8xy needs 1 BYTE
    utf8[0] = codepoint;
    return 1;
  }//is x1100->11ff below this?!?! when given codepoint 1100, it thingks its in 2 lenght, but its in 3
  if (codepoint <= 0x7FF) {//if the codepoint is smaller than 2078, it only needs 2 BYTE
    utf8[0] = 0xC0 /*removes the 2 octet-lenght character boundary*/| (codepoint >> secondplacedigit); //first byte is 0xC0 |ored against second place digit
    utf8[1] = 0x80 /*binary 10000000 */| (codepoint & mask/*binary 111111*/);
    return 2;
  }
  if (codepoint <= 0xFFFF) {
    if (codepoint >= 0xD800 && codepoint <= 0xDFFF) {
      return 0; //these code blocks are reserved for UTF-16 data. 
    }
    utf8[0] = 0xE0 /*removes the 3 octet-lenght character boundary*/ | (codepoint >> thirdplacedigit);
    utf8[1] = 0x80 /*binary 10000000 */| ((codepoint >> secondplacedigit) & mask/*binary 111111*/);
    utf8[2] = 0x80 /*binary 10000000 */| (codepoint & mask/*binary 111111*/);
    return 3;
  }
  if (codepoint <= 0x10FFFF) {
    utf8[0] = 0xF0 /*removes the 4 octet-lenght character boundary*/| (codepoint >> fourthplacedigit);
    utf8[1] = 0x80 /*removes the octet start marker from each following octet*/| ((codepoint >> thirdplacedigit) & mask/*binary 111111*/);
    utf8[2] = 0x80 /*binary 10000000 removes the octet start marker from each following octet*/| ((codepoint >> secondplacedigit) & mask/*binary 111111*/);
    utf8[3] = 0x80 /*binary 10000000 removes the octet start marker from each following octet*/| (codepoint & mask/*binary 111111*/);
    return 4;
  }
  return 0;
}

String HandleBadCodePoint(uint32_t codepoint){
  /*A simple error handling that currently does basically nothing, called in case Unicode_codepoint_to_utf8 throws 0 (which it 
   * will do if the thing passed in is not a codepoint in the valid range x0-x10FFFF, or (and) if the code is between 
   * 0xD800 and 0xDFFF, which are reserved UTF16 codepoints. 
   */
  String error = String("the codepoint is not valid");
  return error;
}

const char* keyboard = "2";
//hangul_init();
HangulInputContext *hic = hangul_ic_new(keyboard);

void setup() {
    Serial.begin(9600);
     
}

void loop() {
  Serial.printf("Hello there?!?\n");
  Serial.printf("the strlen is %d", strlen(input));
  // put your main code here, to run repeatedly:
  uint8_t utf8[4]={0,0,0,0};
  String result = "";
  String single;
  int ascii;

  for(int x=0; x <= strlen(input); x++){
        
        Serial.printf("the iteration is %d\n",x);
        Serial.printf("the input[x] is %c\n", input[x]);
        int ret = hangul_ic_process(hic, input[x]);//handles backspaces and keyboards
        if (!ret) {//if hic == NULL, hangul_ic_process returns false
          printf("%c", ascii);
        }
        
        Serial.printf("confirm iteration %d \n", x);
        const ucschar *get_commit = hangul_ic_get_commit_string(hic);
        
        unsigned len = Unicode_CodepointToUTF8(utf8, *get_commit);
        Serial.printf("number of octets: %d \n", len);
        if (len == 0 ){
          Serial.print(HandleBadCodePoint(*get_commit));
        }
        //Serial.print(String((char *)utf8));
        Serial.printf("code is %x %x %x %x\n", utf8[0], utf8[1], utf8[2], utf8[3]);
        char* single = (char *)utf8;
        //Serial.printf("printing single character: %c \n", single);
        result = result+single;
        //Serial.print("string in consutrction: ");
        Serial.println(result);
        
    }
    Serial.println(result);

    hangul_ic_flush(hic);
    
//    String result = String(cstr);
//
    hangul_ic_reset(hic);
    hangul_ic_delete(hic);

    Serial.print("Input:   ");
    Serial.println(input);
    Serial.print("Result:  ");
    Serial.println(result);
    Serial.print("Correct: ");
    Serial.println(correct);

    delay(6000);

} 
