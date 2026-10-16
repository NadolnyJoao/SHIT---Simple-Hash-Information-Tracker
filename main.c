#include <stdio.h>
#include <string.h>
void RenderLogs(){
  printf("mensagem hash \nAuthor: me\nData: 16-10-2026\n\n   Name mensagem ");
}


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
