#include <ijvm.h>
#include <stdio.h>
#include <unistd.h>

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
fread(&magicn,sizeof(uint32_t), 1, fp);
magicn = swap_uint32(magicn);
printf("%x \n", magicn);


fread(buffer, sizeof(char), 128, fp);

return 1;
}

void destroy_ijvm()
{
  // Reset IJVM state
}

void run()
{
  // Step while you can
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

return 0;
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


