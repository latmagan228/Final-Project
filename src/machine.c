#include <ijvm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct gvariables
{
uint8_t* text;
uint32_t tsize;
int counter;
};

struct gvariables gv;

static uint32_t swap_uint32(uint32_t num)
{
  return((num>>24)&0xff) | ((num<<8)&0xff0000) |((num>>8)&0xff00) | ((num<<24)&0xff000000);
}

int init_ijvm(char *binary_file)
{
  char *binary = binary_file;
  FILE *fp;
  fp = fopen(binary, "rb");

  uint32_t magicn = 0; 
  fread(&magicn, sizeof(uint32_t), 1, fp);
  magicn = swap_uint32(magicn);

  uint32_t cporigin;
  fread(&cporigin, sizeof(uint32_t), 1, fp);
  cporigin = swap_uint32(cporigin);

  uint32_t cpsize;
  fread(&cpsize, sizeof(uint32_t), 1, fp);
  cpsize = swap_uint32(cpsize);

  uint32_t* cpdata = (uint32_t*) malloc(cpsize/4*sizeof(uint32_t));
  for(int i = 0; i < cpsize/4; i++) 
  {
    fread(&cpdata[i], sizeof(uint32_t), 1, fp);
    cpdata[i] = swap_uint32(cpdata[i]);
  } 

  uint32_t torigin;
  fread(&torigin, sizeof(uint32_t), 1, fp);
  torigin = swap_uint32(torigin);

  fread(&gv.tsize, sizeof(uint32_t), 1, fp);
  gv.tsize = swap_uint32(gv.tsize);

  uint8_t* tdata = (uint8_t*) malloc(gv.tsize*sizeof(uint8_t));
  fread(tdata, sizeof(char), gv.tsize, fp);

  gv.text = tdata;
  fclose(fp);

  return 1;
}

void destroy_ijvm()
{
  free(gv.text);
  gv.tsize = 0;
  gv.counter = 0;
}

void run()
{
  while(step())
  {
  switch (gv.text[gv.counter]) {
    case OP_BIPUSH:
      gv.counter += 1;
      printf("BIPUSH\n");
      break;
    case OP_IADD:
      printf("IADD\n");
      break;
    case OP_OUT:
      printf("OUT\n");
      break;
    case OP_NOP:
      printf("NOP\n");
      break;
    case OP_LDC_W:
      gv.counter += 2;
      printf("LDC_W\n");
      break;
    case OP_DUP:
      printf("DUP\n");
      break;
    case OP_HALT:
      break;
  }
  gv.counter += 1;
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

return gv.tsize;
}

bool step(void){

  if (gv.text[gv.counter] == OP_HALT)
  {
    return false;
  }
  else{ return true;}

}

byte_t *get_text(void){

return gv.text;
}

int get_program_counter(void){

return gv.counter;
}

byte_t get_instruction(void){

  return gv.text[gv.counter];
}


