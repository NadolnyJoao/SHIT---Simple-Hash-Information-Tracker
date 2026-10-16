#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>


int main(int  argc, char * argv[]) {

  if(argc<2){
    printf("SHIT - A good version of Git");
    return 0;
  }
  if( strcmp(argv[1],"log")==0){
    RenderLogs();
  }
  return 0;
}
void cmd_init() { mkdir(".shit", 0777); }
void RenderLogs(){
  printf("mensagem hash \nAuthor: me\nData: 16-10-2026\n\n   Name mensagem ");
}