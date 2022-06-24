#include <ijvm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#define STACK_SIZE 4096

struct gvariables
{
uint8_t* text;
uint32_t tsize;
int counter;
word_t *stack;
int ssize;
int sp;
int lvsp;
FILE *fp;
FILE *out;
FILE *in;
word_t* cvar;
};

struct gvariables gv;

static uint32_t swap_uint32(uint32_t num)
{
  return((num>>24)&0xff) | ((num<<8)&0xff0000) |((num>>8)&0xff00) | ((num<<24)&0xff000000);
}

int init_ijvm(char *binary_file)
{
  gv.out = stdout;
  gv.in = stdin;
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

  word_t* cpdata = (word_t*) malloc(cpsize/4*sizeof(word_t));
  for(int i = 0; i < cpsize/4; i++) 
  {
    fread(&cpdata[i], sizeof(uint32_t), 1, gv.fp);
    cpdata[i] = swap_uint32(cpdata[i]);
  } 
  gv.cvar = cpdata;

  uint32_t torigin;
  fread(&torigin, sizeof(uint32_t), 1, gv.fp);
  torigin = swap_uint32(torigin);

  fread(&gv.tsize, sizeof(uint32_t), 1, gv.fp);
  gv.tsize = swap_uint32(gv.tsize);

  uint8_t* tdata = (uint8_t*) malloc(gv.tsize*sizeof(uint8_t));
  fread(tdata, sizeof(char), gv.tsize, gv.fp);
  gv.text = tdata;

  gv.stack = (word_t*)malloc(STACK_SIZE);
  gv.sp = 20;
  gv.lvsp = 0;

  return 1;
}

void destroy_ijvm()
{
  free(gv.text);
  free(gv.stack);
  gv.sp = 0;
  gv.lvsp = 0;
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
  //gv.in = stdin;
} 

void set_output(FILE *fp)
{
  gv.out = fp;
  //gv.out = stdout;
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

word_t get_constant(int i) {

  return gv.cvar[i];
}

word_t get_local_variable(int i) {
   
  return gv.stack[gv.lvsp + i];
}

int16_t byte_to_int(){
  byte_t *bytes = &gv.text[gv.counter + 1];
  int16_t result = (int16_t)((bytes[0] << 8) | (bytes[1]));
  return result;
}

bool step(void){
  byte_t a;
  byte_t b;
  int16_t c;
  int d;
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
      gv.counter += c - 1;
      printf("%d\n",  gv.counter);
      break;
    case OP_IFEQ:
      if (pop() == 0) {
        printf("IFEQ");
        c = byte_to_int();
        gv.counter += c - 1;
        printf("%d\n",  gv.counter);
        break;
      }
      else {
        gv.counter += 2;
        break;
      }
    case OP_IFLT:      
      if (pop() < 0) {
        printf("IFLT\n");
        c = byte_to_int();
        gv.counter += c - 1;
        printf("%d\n",  gv.counter);
        break;
      }
      else {
        gv.counter += 2;
        break;
      }
    case OP_ICMPEQ:      
      if (pop() == pop()) {
        printf("ICMPEQ\n");
        c = byte_to_int();
        gv.counter += c - 1;
        printf("%d\n",  gv.counter);
        break;
      }
      else {
        gv.counter += 2;
        break;
      }
    case OP_NOP:
      printf("NOP\n");
      break;
    case OP_OUT:
      printf("OUT\n");
      putc(pop(), gv.out);
      break;
    case OP_IN:
      printf("OUT\n");
      push(getc(gv.in));
      break;
    case OP_LDC_W:
      printf("LDC_W\n");
      c = byte_to_int();
      push((int8_t)get_constant(c));
      printf("%x\n", (int8_t)get_constant(c));
      gv.counter += 2;
      break;
    case OP_DUP:
      printf("DUP\n");
      printf("%x\n", (int8_t)gv.stack[gv.sp]);
      push((int8_t)gv.stack[gv.sp]);
      break;
    case OP_ISTORE:
      printf("ISTORE\n");
      a = pop();
      d = (int8_t)gv.text[gv.counter + 1];
      gv.stack[gv.lvsp + d] = a;
      printf("%x\n", (int8_t)gv.stack[gv.sp]);
      gv.counter += 1;
      break;
    case OP_ILOAD:
      printf("ILOAD\n");
      d = (int8_t)gv.text[gv.counter + 1];
      a = get_local_variable(d);
      push(a);
      printf("%x\n", (int8_t)gv.stack[gv.sp]);
      gv.counter += 1;
      break;
    case OP_IINC:
      printf("IINC\n");
      d = (int8_t)gv.text[gv.counter + 1];
      b = (int8_t)gv.text[gv.counter + 2];
      a = get_local_variable(d);
      gv.stack[gv.lvsp + d] = (int8_t)(a + b);
      gv.counter += 2;
      printf("%x\n", get_local_variable(d));
      break;
    case OP_WIDE:
      printf("WIDE\n");
      step();
      break;
    case OP_HALT:
      return false;
      break;
  }
  gv.counter += 1;
  if (gv.counter == gv.tsize) {
    return false;
  }
  a = 0;
  b = 0;
  c = 0;
  d = 0;
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
