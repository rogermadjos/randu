#include <node.h>
#include <v8.h>
#include <ctime>
#include <fstream>
#include <algorithm>

class MTRand_int32{
public:
  // default constructor: uses default seed only if this is the first instance
  MTRand_int32() { if (!init) seed(5489UL); init = true; }
  // constructor with 32 bit int as seed
  MTRand_int32(unsigned long s) { seed(s); init = true; }
  // constructor with array of size 32 bit ints as seed
  MTRand_int32(const unsigned long* array, int size) { seed(array, size); init = true; }
  // the two seed functions
  void seed(unsigned long); // seed with 32 bit integer
  void seed(const unsigned long*, int size); // seed with array
  // overload operator() to make this a generator (functor)
  unsigned long operator()() { return rand_int32(); }
  virtual ~MTRand_int32() {} // destructor
protected: // used by derived classes, otherwise not accessible; use the ()-operator
  unsigned long rand_int32(); // generate 32 bit random integer
private:
  static const int n = 624, m = 397; // compile time constants
  // the variables below are static (no duplicates can exist)
  static unsigned long state[n]; // state vector array
  static int p; // position in state array
  static bool init; // true if init function is called
  // private functions used to generate the pseudo random numbers
  unsigned long twiddle(unsigned long, unsigned long); // used by gen_state()
  void gen_state(); // generate new state
  // make copy constructor and assignment operator unavailable, they don't make sense
  MTRand_int32(const MTRand_int32&); // copy constructor not defined
  void operator=(const MTRand_int32&); // assignment operator not defined
};

inline unsigned long MTRand_int32::twiddle(unsigned long u, unsigned long v) {
  return (((u & 0x80000000UL) | (v & 0x7FFFFFFFUL)) >> 1) ^ ((v & 1UL) ? 0x9908B0DFUL : 0x0UL);
}

inline unsigned long MTRand_int32::rand_int32() { // generate 32 bit random int
  if (p == n) gen_state(); // new state vector needed
  // gen_state() is split off to be non-inline, because it is only called once
  // in every 624 calls and otherwise irand() would become too big to get inlined
  unsigned long x = state[p++];
  x ^= (x >> 11);
  x ^= (x << 7) & 0x9D2C5680UL;
  x ^= (x << 15) & 0xEFC60000UL;
  return x ^ (x >> 18);
}

unsigned long MTRand_int32::state[n] = {0x0UL};
int MTRand_int32::p = 0;
bool MTRand_int32::init = false;

void MTRand_int32::gen_state(){ // generate new state vector
  for(int i = 0; i < (n - m); ++i){
    state[i] = state[i + m] ^ twiddle(state[i], state[i + 1]);    
  }
  for(int i = n - m; i < (n - 1); ++i){
    state[i] = state[i + m - n] ^ twiddle(state[i], state[i + 1]);
  }
  state[n - 1] = state[m - 1] ^ twiddle(state[n - 1], state[0]);
  p = 0; // reset position
}

void MTRand_int32::seed(unsigned long s){  // init by 32 bit seed
  state[0] = s & 0xFFFFFFFFUL; // for > 32 bit machines
  for (int i = 1; i < n; ++i) {
    state[i] = 1812433253UL * (state[i - 1] ^ (state[i - 1] >> 30)) + i;
    // see Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier
    // in the previous versions, MSBs of the seed affect only MSBs of the array state
    state[i] &= 0xFFFFFFFFUL; // for > 32 bit machines
  }
  p = n; // force gen_state() to be called for next random number
}

void MTRand_int32::seed(const unsigned long* array, int size){ // init by array
  seed(19650218UL);
  int i = 1, j = 0;
  for (int k = ((n > size) ? n : size); k; --k){
    state[i] = (state[i] ^ ((state[i - 1] ^ (state[i - 1] >> 30)) * 1664525UL)) + array[j] + j; // non linear
    state[i] &= 0xFFFFFFFFUL; // for > 32 bit machines
    ++j; 
    j %= size;
    if ((++i) == n){ 
      state[0] = state[n - 1]; i = 1; 
    }
  }
  for (int k = n - 1; k; --k){
    state[i] = (state[i] ^ ((state[i - 1] ^ (state[i - 1] >> 30)) * 1566083941UL)) - i;
    state[i] &= 0xFFFFFFFFUL; // for > 32 bit machines
    if ((++i) == n){ 
      state[0] = state[n - 1]; i = 1; 
    }
  }
  state[0] = 0x80000000UL; // MSB is 1; assuring non-zero initial array
  p = n; // force gen_state() to be called for next random number
}

#ifdef _WIN32
  #define WIN32_LEAN_AND_MEAN
  #include <Windows.h>
  #include <Wincrypt.h>
  #pragma lib("Advapi32.lib")
#endif

using namespace v8;

MTRand_int32 irand;

#ifdef _WIN32

uint32_t random_seed(){
  uint32_t seedValue = 0;
  
  HCRYPTPROV hCryptProv = NULL;
  LPCSTR UserName = "MyKeyContainer";
  
  if (!CryptAcquireContext(&hCryptProv, UserName, NULL, PROV_RSA_FULL, 0)){
    if (GetLastError() == NTE_BAD_KEYSET){
      if (!CryptAcquireContext(&hCryptProv, UserName, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET)){
        exit(1);
      }
    }
  }
  
  if(!CryptGenRandom(hCryptProv, sizeof(seedValue), (BYTE *)&seedValue)){
    exit(3);
  }
  
  if(!CryptReleaseContext(hCryptProv, 0)){
    exit(2);
  }
  
  return seedValue;
}

#else

uint32_t random_seed()
{
  uint32_t a = 0;
  uint32_t b = 0;

  std::ifstream file("/dev/urandom", std::ios::binary);
  if (file.is_open()){
    file.read((char *)&a, sizeof(a));
    file.close();
  }

  b = std::time(0);
  return a ^ b;
}

#endif

Handle<Value> MTRand(const Arguments& args)
{
  HandleScope scope;

  if (args.Length() > 2){
    return ThrowException(Exception::TypeError(String::New("Random cannot support more than 2 arguments")));
  }

  uint32_t randValue = irand();

  if (args.Length() == 1){
    if (!args[0]->IsUint32()){
      return ThrowException(Exception::TypeError(String::New("Range must be a 32 bit unsigned integer")));
    }

    randValue = randValue % args[0]->Uint32Value();
  }

  if (args.Length() == 2){
    if (!args[0]->IsUint32() || !args[1]->IsUint32()){
      return ThrowException(Exception::TypeError(String::New("Max value and range must be 32 bit unsigned intergers")));
    }

    if (args[1]->Uint32Value() < args[0]->Uint32Value()){
      return ThrowException(Exception::TypeError(String::New("The Max value cannot be less than the Min Value")));
    }

    randValue = (randValue % (args[1]->Uint32Value() - args[0]->Uint32Value())) + args[0]->Uint32Value();
  }

  return scope.Close(Number::New(randValue));
}

Handle<Value> MTRand_float(const Arguments& args)
{
  HandleScope scope;

  if (args.Length() > 2){
    return ThrowException(Exception::TypeError(String::New("Random cannot support more than 2 arguments")));
  }

  double randValue = static_cast<double>(irand()) * (1. / 4294967296.);

  if (args.Length() == 1){
    if (!args[0]->IsNumber()){
      return ThrowException(Exception::TypeError(String::New("Range must be a number")));
    }

    randValue = randValue * args[0]->NumberValue();
  }

  if (args.Length() == 2){
    if (!args[0]->IsNumber() || !args[1]->IsNumber()){
      return ThrowException(Exception::TypeError(String::New("Max value and min value must be numbers")));
    }

    if (args[1]->NumberValue() < args[0]->NumberValue()){
      return ThrowException(Exception::TypeError(String::New("The Max value cannot be less than the Min Value")));
    }

    randValue = args[0]->NumberValue() + randValue * (args[1]->NumberValue() - args[0]->NumberValue());
  }

  return scope.Close(Number::New(randValue));
}

void init(Handle<Object> exports)
{
  irand.seed(random_seed());

  exports->Set(String::NewSymbol("rand"),
      FunctionTemplate::New(MTRand)->GetFunction());

  exports->Set(String::NewSymbol("randF"),
    FunctionTemplate::New(MTRand_float)->GetFunction());
}

NODE_MODULE(mtrand, init)