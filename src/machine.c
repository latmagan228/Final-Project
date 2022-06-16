#include <ijvm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

uint8_t* text;
uint32_t tsize;

static uint32_t swap_uint32(uint32_t num)
{
  return((num>>24)&0xff) | ((num<<8)&0xff0000) |((num>>8)&0xff00) | ((num<<24)&0xff000000);
}

int init_ijvm(char *binary_file)
{
  char *binary = "files/task1/program1.ijvm";
  FILE *fp;
  fp = fopen(binary, "rb");
  char buffer[128];

  uint32_t magicn = 0; 
  fread(&magicn, sizeof(uint32_t), 1, fp);
  magicn = swap_uint32(magicn);
  printf("%x \n", magicn);

  uint32_t cporigin;
  fread(&cporigin, sizeof(uint32_t), 1, fp);
  cporigin = swap_uint32(cporigin);
  printf("%x\n", cporigin);

  uint32_t cpsize;
  fread(&cpsize, sizeof(uint32_t), 1, fp);
  cpsize = swap_uint32(cpsize);
  printf("%x\n", cpsize);

  uint32_t* cpdata = (uint32_t*) malloc(cpsize/4*sizeof(uint32_t));
  for(int i = 0; i < cpsize/4; i++) 
  {
    fread(&cpdata[i], sizeof(uint32_t), 1, fp);
    cpdata[i] = swap_uint32(cpdata[i]);
    printf("%x\n", cpdata[i]);
  } 

  uint32_t torigin;
  fread(&torigin, sizeof(uint32_t), 1, fp);
  torigin = swap_uint32(torigin);
  printf("%x\n", torigin);

  fread(&tsize, sizeof(uint32_t), 1, fp);
  tsize = swap_uint32(tsize);
  printf("%x\n", tsize);

  uint8_t* tdata = (uint8_t*) malloc(tsize*sizeof(uint8_t));


  text = tdata;

  fread(tdata, sizeof(char), tsize, fp);
  fclose(fp);

  return 1;
}

void destroy_ijvm()
{
  free(text);
}

void run()
{
  for (int i = 0; i < tsize; i++)
  {
    switch (text[i]) {
      case OP_BIPUSH:
        printf("BIPUSH\n");
        break;
      case OP_IADD:
        printf("IADD\n");
        break;
      case OP_OUT:
        printf("OUT\n");
        break;
    }
  }

}

void set_input(FILE *fp)
{
  // TODO: implement me
}

void set_output(FILE *fp)
{
  // TODO: implement me
}

int text_size(void){

return tsize;
}

bool step(void){

return false;
}

byte_t *get_text(void){

return NULL;
}

int get_program_counter(void){

return 0;
}


