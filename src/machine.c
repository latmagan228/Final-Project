#include <ijvm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#define STACK_SIZE 70000

struct gvariables
{
uint8_t* text;
uint32_t tsize;
int counter;
int lvcounter;
word_t *stack;
int ssize;
int sp;
int lvsp;
FILE *fp;
FILE *out;
FILE *in;
word_t* cvar;
int nfunctions;
int currentcounter[50];
int currentsp;
int functioncounter;
bool wide;
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
  gv.sp = 256;
  gv.lvsp = 0;
  gv.wide = false;

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
  gv.lvsp = 0;
  gv.lvcounter = 0;
  gv.functioncounter = 0;
}

void run()
{
  while(step());
}

bool finished(void) {
  if (gv.text[gv.counter] == OP_ERR){
    return false;
  }
  else if (gv.text[gv.counter] == OP_HALT){
    return false;
  }
  else if (gv.counter == gv.tsize) {
    return false;
  }
  else {
  return true;
  }
}

void set_input(FILE *file)
{
  gv.in = file;
} 

void set_output(FILE *file)
{
  gv.out = file;
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

int16_t short_to_int(){
  byte_t *bytes = &gv.text[gv.counter + 1];
  int16_t result = (int16_t)((bytes[0] << 8) | (bytes[1]));
  return result;
}

bool step(void){
  byte_t a;
  byte_t b;
  int16_t c;
  int16_t nlv;
  int16_t narg;
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
      c = short_to_int();
      gv.counter += c - 1;
      printf("%d\n",  gv.counter);
      break;
    case OP_IFEQ:
      if (pop() == 0) {
        printf("IFEQ");
        c = short_to_int();
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
        c = short_to_int();
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
        c = short_to_int();
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
      printf("IN\n");
      int getin;
      getin = fgetc(gv.in);
      if(getin == EOF){
        push(0);
      }
      else {
      push(getin);
      }
      break;
    case OP_LDC_W:
      printf("LDC_W\n");
      c = short_to_int();
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
      if (gv.wide) {
        byte_t *bytes = &gv.text[gv.counter + 1];
        uint16_t result = (uint16_t)((bytes[0] << 8) | (bytes[1]));
        d =  result;
        printf("res %d\n", result);
      }
      else {
        d = (int8_t)gv.text[gv.counter + 1];
      }
      gv.stack[gv.lvsp + d] = a;
      printf("%x\n", (int8_t)gv.stack[gv.sp]);
      gv.lvcounter += 1;
      gv.counter += 1;
      gv.wide = false;
      break;
    case OP_ILOAD:
      printf("ILOAD\n");
      if (gv.wide) {
        byte_t *bytes = &gv.text[gv.counter + 1];
        uint16_t result = (uint16_t)((bytes[0] << 8) | (bytes[1]));
        d =  result;
      }
      else {
        d = (int8_t)gv.text[gv.counter + 1];
      }
      a = get_local_variable(d);
      push(a);
      printf("%x\n", (int8_t)gv.stack[gv.sp]);
      gv.counter += 1;
      gv.wide = false;
      break;
    case OP_IINC:
      printf("IINC\n");
      if (gv.wide) {
        byte_t *bytes = &gv.text[gv.counter + 1];
        uint16_t result = (uint16_t)((bytes[0] << 8) | (bytes[1]));
        d =  result;
      }
      else {
        d = (int8_t)gv.text[gv.counter + 1];
      }
      b = (int8_t)gv.text[gv.counter + 2];
      a = get_local_variable(d);
      gv.stack[gv.lvsp + d] = (int8_t)(a + b);
      gv.counter += 2;
      printf("%x\n", get_local_variable(d));
      gv.wide = false;
      break;
    case OP_INVOKEVIRTUAL:
      printf("INVOKEVIRTUAL\n");
      c = short_to_int();
      gv.nfunctions += 1;
      gv.currentcounter[gv.nfunctions] = gv.counter + 2;
      gv.counter = get_constant(c);
      gv.counter -= 1;
      narg = short_to_int();
      gv.functioncounter = narg;
      printf("%d\n", narg);
      gv.counter += 2;
      nlv = short_to_int();
      printf("%d\n", nlv);
      gv.counter += 2;
      printf("%d\n", gv.lvsp);
      gv.lvsp += gv.functioncounter;
      if(gv.lvsp == gv.functioncounter) {
        gv.lvsp = gv.lvcounter;
      }
      else {
        gv.lvsp += 1;
      }
      for(int i = 0; i < narg; i++) {
          a= pop();
          gv.stack[narg - i - 1 + gv.lvsp] = a;
          printf("%d\n", narg - i - 1 + gv.lvsp);
      }
      printf("%d\n", gv.lvsp);
      break;
    case OP_IRETURN:
      printf("IRETURN\n");
      gv.counter = gv.currentcounter[gv.nfunctions--];
      gv.lvsp -= gv.functioncounter + 1;
      if (gv.lvsp + gv.functioncounter + 1 == gv.lvcounter){
        gv.lvsp = 0;
      }
      printf("%d\n", gv.lvsp);
      printf("%d\n", gv.counter);
      break;
    case OP_WIDE:
      printf("WIDE\n");
      gv.wide = true;
      gv.counter += 1;
      step();
      break;
    case OP_ERR:
      printf("there has been an error");
      return false;
      break;
    case OP_HALT:
      printf("HALT");
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
