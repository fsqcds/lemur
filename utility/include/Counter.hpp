#ifndef _COUNTER_HPP
#define _COUNTER_HPP


/// Abstract Counter class
/*!
  The double type, rather than int type, is used so that it may
be useful in a setting like EM.
  The event index has a range [0,N].
  The caller is responsible for boundary check if necessary. 
 */

class Counter {
public:

  // access functions
  
  /// return the count of a word
  virtual double count(int eventIndex) = 0;
  /// return the sum of all counts
  virtual double sum() = 0;
  
  /// iteration over non-zero counts
  virtual void startIteration() = 0;
  virtual bool hasMore() = 0;
  virtual void nextCount(int &eventIndex, double &count) = 0;
};

/// Modifiable counter, supports modification of counts
class ModifiableCounter : public Counter {
public: 
  // manipulation functions
  virtual void incCount(int eventIndex, double count) = 0;
  virtual void setCount(int eventIndex, double count)  = 0;
};

/// Counts stored in an array

template<class T>
class ArrayCounter : public ModifiableCounter {
public:
  
  ArrayCounter(int size) : sz(size), ct(new T[size]), total(0) {
    for (int i=0; i<size; i++) ct[i]=0;
  }
  ~ArrayCounter() { delete [] ct;}

  /// return the count of an event
  virtual double count(int eventIndex) {
    return ct[eventIndex];
  }

  /// return the sum of all counts
  virtual double sum() {
    return total;
  }

  virtual void incCount(int eventIndex, double count) {
    ct[eventIndex] += (T)count;
    total += count;
  }

  virtual void setCount(int eventIndex, double count) {
    total -= ct[eventIndex]+ (T)count;
    ct[eventIndex] = (T)count;
  }


  virtual void startIteration() {
    pos=0;
  }

  virtual bool hasMore() {
    while ((pos < sz) && (ct[pos] == 0))
      pos++;
    return (pos<sz); 
  }

  virtual void nextCount(int &eventIndex, double &count) {
    eventIndex = pos;
    count = ct[pos];
    pos++;
  }

protected:
  T *ct;
  int sz;
  T total;
  int pos;
};

#endif /* _COUNTER_HPP */
