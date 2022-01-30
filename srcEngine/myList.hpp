#pragma once
#include <string.h>
#include <stdexcept>
template <typename T> class myList{
public:
  myList(unsigned int nAlloc){
    nAlloc = nAlloc < 1 ? 1 : nAlloc;
    this->nAlloc = nAlloc;
    this->mem = (T*)malloc(this->nAlloc * sizeof(T));
  }

  ~myList(){
    free(this->mem);    
  }
  
  void push(T elem){
    if (this->nAlloc == this->nElem){
      this->nAlloc *= 2;
      this->mem = (T*)realloc(this->mem, this->nAlloc * sizeof(T));
      if (this->mem == NULL) throw std::runtime_error("realloc failed");
    }
    *(this->mem + this->nElem) = elem;
    ++this->nElem;
  }

  T operator[](unsigned int ix) const{
    return *(this->mem+ix);
  }
  
  unsigned int length() const{
    return this->nElem;
  }

  T* pointer() const{
    return this->mem;
  }

  unsigned int getNBytes() const{
    return this->nElem * sizeof(T);
  }

  void clear(){
    this->nElem = 0; // note: keep memory size as is
  }
  
private:
  unsigned int nElem = 0;
  unsigned int nAlloc = 0;
  T* mem;
};
