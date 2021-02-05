#include <hangul.h>

void setup() {
  // put your setup code here, to run once:
  const char* keyboard = NULL;
  int ascii = 32;
  int len;
  const ucschar *str;
  HangulInputContext *hic;
  
  hangul_ic_new(keyboard);
  hangul_ic_process(hic, ascii);
  str = hangul_ic_flush(hic);
  hangul_ic_backspace(hic);
  str = hangul_ic_get_preedit_string(hic);
  str = hangul_ic_get_commit_string(hic);
  hangul_ic_reset(hic);
  hangul_ic_delete(hic);
}

void loop() {
  // put your main code here, to run repeatedly:

} 
