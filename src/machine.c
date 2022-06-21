#include <ijvm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#define STACK_SIZE 1024

struct gvariables
{
uint8_t* text;
uint32_t tsize;
int counter;
word_t stack[STACK_SIZE];
int ssize;
int sp;
FILE *fp;
FILE *out;
FILE *in;
};

struct gvariables gv;

static uint32_t swap_uint32(uint32_t num)
{
  return((num>>24)&0xff) | ((num<<8)&0xff0000) |((num>>8)&0xff00) | ((num<<24)&0xff000000);
}

int init_ijvm(char *binary_file)
{
  char *binary = binary_file;
  gv.fp = fopen(binary, "rb");

  uint32_t magicn = 0; 
  fread(&magicn, sizeof(uint32_t), 1, gv.fp);
  magicn = swap_uint32(magicn);

  uint32_t cporigin;
  fread(&cporigin, sizeof(uint32_t), 1, gv.fp);
  cporigin = swap_uint32(cporigin);

  uint32_t cpsize;
  fread(&cpsize, sizeof(uint32_t), 1, gv.fp);
  cpsize = swap_uint32(cpsize);

  uint32_t* cpdata = (uint32_t*) malloc(cpsize/4*sizeof(uint32_t));
  for(int i = 0; i < cpsize/4; i++) 
  {
    fread(&cpdata[i], sizeof(uint32_t), 1, gv.fp);
    cpdata[i] = swap_uint32(cpdata[i]);
  } 

  uint32_t torigin;
  fread(&torigin, sizeof(uint32_t), 1, gv.fp);
  torigin = swap_uint32(torigin);

  fread(&gv.tsize, sizeof(uint32_t), 1, gv.fp);
  gv.tsize = swap_uint32(gv.tsize);

  uint8_t* tdata = (uint8_t*) malloc(gv.tsize*sizeof(uint8_t));
  fread(tdata, sizeof(char), gv.tsize, gv.fp);
  gv.text = tdata;

  return 1;
}

void destroy_ijvm()
{
  free(gv.text);
  gv.sp = 0;
  gv.tsize = 0;
  gv.counter = 0;
  fclose(gv.fp);
}

void run()
{
  while(step());
}

void set_input(FILE *fp)
{
  gv.in = fp;
  gv.in = stdin;
} 

void set_output(FILE *fp)
{
  gv.out = fp;
  gv.out = stdout;
}

int text_size(void){

return gv.tsize;
}

word_t push(word_t n) {
  assert(gv.sp < STACK_SIZE -1);
  gv.stack[++gv.sp] = n;
  return gv.stack[gv.sp];
}

word_t pop() {
  assert(gv.sp >= 0);
  return gv.stack[gv.sp--];
}

int byte_to_int(){
  byte_t *bytes = &gv.text[gv.counter + 1];
  int result = (bytes[0] & 0xFF << 8) + (bytes[1] & 0xFF);
  return result;
}

bool step(void){
  byte_t a;
  byte_t b;
  int c;
  switch (gv.text[gv.counter]) {
    case OP_BIPUSH:
      printf("BIPUSH\n");
      push((int8_t)gv.text[gv.counter + 1]);
      printf("%x\n", (int8_t)gv.stack[gv.sp]);
      gv.counter += 1;
      break;
    case OP_IADD:
      a = pop();
      b = pop();
      push(b + a);
      printf("IADD\n");
      printf("%x\n", (int8_t)gv.stack[gv.sp]);
      break;
    case OP_ISUB:
      a = pop();
      b = pop();
      push(b - a);
      printf("ISUB\n");
      printf("%x\n", (int8_t)gv.stack[gv.sp]);
      break;
    case OP_IAND:
      a = pop();
      b = pop();
      push(b & a);
      printf("IAND\n");
      printf("%x\n", (int8_t)gv.stack[gv.sp]);
      break;
    case OP_IOR:
      a = pop();
      b = pop();
      push(b | a);
      printf("IOR\n");
      printf("%x\n", (int8_t)gv.stack[gv.sp]);
      break;
    case OP_POP:
      pop();
      printf(("POP\n"));
      printf("%x\n", (int8_t)gv.stack[gv.sp]);
      break;
    case OP_SWAP:
      a = pop();
      b = pop();
      push(a);
      push(b);
      printf("ISWAP\n");
      printf("%x\n", (int8_t)gv.stack[gv.sp]);
      break;
    case OP_GOTO:
      printf(("GOTO\n"));
      c = byte_to_int();
      printf("%d\n", gv.counter);
      printf("%d\n", c);
      gv.counter += c - 1;
      printf("%d\n",  gv.counter);
      break;
    case OP_OUT:
      printf("OUT\n");
      break;
    case OP_LDC_W:
      gv.counter += 2;
      printf("LDC_W\n");
      break;
    case OP_DUP:
      printf("DUP\n");
      break;
    case OP_HALT:
      return false;
      break;
  }
  gv.counter += 1;
  if (gv.counter == gv.tsize) {
    return false;
  }
  return true;
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

word_t tos(void){

  return (int8_t)gv.stack[gv.sp];
}

word_t *get_stack(void){

  return 0;
}

int stack_size(void){

  return gv.sp;
}
