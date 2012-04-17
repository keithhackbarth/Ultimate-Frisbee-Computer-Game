#include "PortMutex.h"
#include "CarnegieMellonGraphics.h"
#include "Debugging.h"

/*
ofstream logfile(CMUGFX_LOGFILE);

void log_message(string message, string file, long line) {
  logfile    << "Message in " << file << " @ line " << line << " : "
       <<  message << "." << endl;

  logfile.flush();
}
*/

//<new>
static int newcount = 0;
static int deletecount = 0;
static void *alloced[100000];
static int newcount2 = 0;
static int deletecount2 = 0;

const int SENTINEL = 0x12efcdab;
const int ALLOCED = 0x0a110ced;
const int DELETED = 0x0de1e1ed;
// header is 4 ints (SENTINEL, size)
// tail is 4 int

inline void checkall();
inline void checknotdeleted(void *p);

inline void *sentinelized(size_t s) {

  void *p = malloc(s+8*sizeof(int));

//  if (p == NULL) throw std::bad_alloc();
  ((int*)p)[0] = SENTINEL;
  ((int*)p)[1] = s;
  ((int*)p)[2] = SENTINEL;
  ((int*)p)[3] = SENTINEL;
  *(int*) ((unsigned char*)p+s+4*sizeof(int)) = SENTINEL;
  bool reassigned = false;
  void *ret = (int*)p+4;
  for (int i = 0; i < newcount; i += 2) {
    if (alloced[i] == ret) {
      reassigned = true;
      fatal_assert(((int*)alloced)[i+1]==ALLOCED,"re-alloced something",__FILE__,__LINE__);
      ((int*)alloced)[i+1] = ALLOCED;
    }
  }
  if (!reassigned) {
    alloced[newcount++] = (int*)p+4;
    ((int*)alloced)[newcount++] = ALLOCED;
  }
  return ret;
}

#ifdef MYNEW
inline void *__cdecl operator new(size_t s) {
  checkall();
  return sentinelized(s);
}

inline void __cdecl operator delete(void *p) {
  if (p == NULL) return;
  checkall();
  deletecount++;
  checknotdeleted(p);

  free((int*)p-4);

}

inline void *__cdecl operator new[](size_t s) {
  checkall();
  return sentinelized(s);
}

inline void __cdecl operator delete[](void *p) {
  if (p == NULL) return;
  checkall();
  deletecount++;
  checknotdeleted(p);

  free((int*)p-4);

}
#endif

inline void checkall() {
//  fatal_assert(newcount < deletecount,"newcount < deletecount",__FILE__,__LINE__);

  for (int i = 0; i < newcount; i+=2) {
    void *p = alloced[i];
    int a = ((int*)alloced)[i+1];
    fatal_assert(!(a == DELETED || a == ALLOCED),"state was trashed", __FILE__, __LINE__);
    if (a != DELETED) {
      int *header = (int*)((int*)p-4);
      fatal_assert(*header != SENTINEL, "beginning sentinel was trashed", __FILE__, __LINE__);
      int size = header[1];
      int *tail = (int*)((unsigned char*)header+size+4*sizeof(int));
      fatal_assert(*tail != SENTINEL, "end sentinel was trashed", __FILE__, __LINE__);
    }
  }
}

inline void checknotdeleted(void *p) {
  bool valid = false;
  int where = 0;
  for (int i = 0; i < newcount; i+=2) {
    if (alloced[i] == p) {
      valid = true;
      where = i;
      break;
    }
  }
  fatal_assert(!valid,"tried to delete invalid pointer",__FILE__,__LINE__);
  fatal_assert(((int*)alloced)[where+1] == DELETED,"tried to delete a pointer twice", __FILE__, __LINE__);
  ((int*)alloced)[where+1] = DELETED;
}



///------------------
/*
JString::JString() { s = 0; str[0] = '\0';}
JString::JString(const char *cstr) { strcpy(str,cstr); s = strlen(str); }
JString::JString(char cstr[]) { strcpy(str,cstr); s = strlen(str); }

JString &JString::operator+=(const JString &rhs) {
  strcat(str,rhs.str);
  s += rhs.s;
  return *this;
}

JString JString::operator +(const JString &rhs) const{
  JString temp(*this);
  temp += rhs;
  return temp;
}

bool JString::operator==(const JString &rhs) const {
  return (strcmp(str,rhs.str)==0);
}

bool JString::operator!=(const JString &rhs) const {
  return !(*this == rhs);
}

bool JString::operator==(const char *rhs) const {
  return strcmp(str,rhs)==0;
}

bool JString::operator!=(const char *rhs) const {
  return !(*this == rhs);
}

bool JString::operator< (const JString &rhs) const {
  return (strcmp(str,rhs.str)==-1);
}


JString::operator const char *() const {
  return str;
}

int JString::length() const { return s; }

const char *JString::c_str() const { return str; }


JString operator+(const char *lhs, const JString &rhs) {
  JString temp(lhs);
  temp += rhs;
  return temp;
}
*/
