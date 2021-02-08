#include <hangul.h>

void setup() {
    char *input = "vkdlTjs gksrmf fkdlqmfjfl xptmxm";
    char *correct = "파이썬 한글 라이브러리 테스트";

    HangulInputContext *hic = hangul_ic_new(NULL);

    for(int x=0; x < strlen(input); x++)
    {
        hangul_ic_process(hic, input[x]);
    }

    hangul_ic_flush(hic);
    const ucschar *ucsstr = hangul_ic_get_commit_string(hic);
    char *cstr = ucschar_to_char(ucstr);
    String result = String(cstr);

    hangul_ic_reset(hic);
    hangul_ic_delete(hic);

    Serial.println("Input:   ", ""input);
    Serial.println("Result:  ", ""result);
    Serial.println("Correct: ", ""correct);
}

void loop() {
  // put your main code here, to run repeatedly:

} 
