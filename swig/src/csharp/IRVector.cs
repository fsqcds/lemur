/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.29
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

namespace Lemur {

using System;
using System.Runtime.InteropServices;

public class IRVector : IDisposable, System.Collections.IEnumerable {
  private HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal IRVector(IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(IRVector obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  ~IRVector() {
    Dispose();
  }

  public virtual void Dispose() {
    if(swigCPtr.Handle != IntPtr.Zero && swigCMemOwn) {
      swigCMemOwn = false;
      lemur_csharpPINVOKE.delete_IRVector(swigCPtr);
    }
    swigCPtr = new HandleRef(null, IntPtr.Zero);
    GC.SuppressFinalize(this);
  }

  public IRVector(System.Collections.ICollection c) : this() {
    if (c == null)
      throw new ArgumentNullException("c");
    foreach (IndexedReal element in c) {
      this.Add(element);
    }
  }

  public bool IsFixedSize {
    get {
      return false;
    }
  }

  public bool IsReadOnly {
    get {
      return false;
    }
  }

  public IndexedReal this[int index]  {
    get {
      return getitem(index);
    }
    set {
      setitem(index, value);
    }
  }

  public int Capacity {
    get {
      return (int)capacity();
    }
    set {
      if (value < size())
        throw new ArgumentOutOfRangeException("Capacity");
      reserve((uint)value);
    }
  }

  public int Count {
    get {
      return (int)size();
    }
  }

  public bool IsSynchronized {
    get {
      return false;
    }
  }

  public void CopyTo(System.Array array) {
    CopyTo(0, array, 0, this.Count);
  }

  public void CopyTo(System.Array array, int arrayIndex) {
    CopyTo(0, array, arrayIndex, this.Count);
  }

  public void CopyTo(int index, System.Array array, int arrayIndex, int count) {
    if (array == null)
      throw new ArgumentNullException("array");
    if (index < 0)
      throw new ArgumentOutOfRangeException("index", "Value is less than zero");
    if (arrayIndex < 0)
      throw new ArgumentOutOfRangeException("arrayIndex", "Value is less than zero");
    if (count < 0)
      throw new ArgumentOutOfRangeException("count", "Value is less than zero");
    if (array.Rank > 1)
      throw new ArgumentException("Multi dimensional array.");
    if (index+count > this.Count || arrayIndex+count > array.Length)
      throw new ArgumentException("Number of elements to copy is too large.");
    for (int i=0; i<count; i++)
      array.SetValue(getitemcopy(index+i), arrayIndex+i);
  }

  // Type-safe version of IEnumerable.GetEnumerator
  System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator() {
    return new IRVectorEnumerator(this);
  }

  public IRVectorEnumerator GetEnumerator() {
    return new IRVectorEnumerator(this);
  }

  // Type-safe enumerator
  /// Note that the IEnumerator documentation requires an InvalidOperationException to be thrown
  /// whenever the collection is modified. This has been done for changes in the size of the
  /// collection but not when one of the elements of the collection is modified as it is a bit
  /// tricky to detect unmanaged code that modifies the collection under our feet.
  public sealed class IRVectorEnumerator : System.Collections.IEnumerator {
    private IRVector collectionRef;
    private int currentIndex;
    private object currentObject;
    private int currentSize;

    public IRVectorEnumerator(IRVector collection) {
      collectionRef = collection;
      currentIndex = -1;
      currentObject = null;
      currentSize = collectionRef.Count;
    }

    // Type-safe iterator Current
    public IndexedReal Current {
      get {
        if (currentIndex == -1)
          throw new InvalidOperationException("Enumeration not started.");
        if (currentIndex > currentSize - 1)
          throw new InvalidOperationException("Enumeration finished.");
        if (currentObject == null)
          throw new InvalidOperationException("Collection modified.");
        return (IndexedReal)currentObject;
      }
    }

    // Type-unsafe IEnumerator.Current
    object System.Collections.IEnumerator.Current {
      get {
        return Current;
      }
    }

    public bool MoveNext() {
      int size = collectionRef.Count;
      bool moveOkay = (currentIndex+1 < size) && (size == currentSize);
      if (moveOkay) {
        currentIndex++;
        currentObject = collectionRef[currentIndex];
      } else {
        currentObject = null;
      }
      return moveOkay;
    }

    public void Reset() {
      currentIndex = -1;
      currentObject = null;
      if (collectionRef.Count != currentSize) {
        throw new InvalidOperationException("Collection modified.");
      }
    }
  }

  public void Clear() {
    lemur_csharpPINVOKE.IRVector_Clear(swigCPtr);
  }

  public void Add(IndexedReal value) {
    lemur_csharpPINVOKE.IRVector_Add(swigCPtr, IndexedReal.getCPtr(value));
    if (lemur_csharpPINVOKE.SWIGPendingException.Pending) throw lemur_csharpPINVOKE.SWIGPendingException.Retrieve();
  }

  private uint size() {
    uint ret = lemur_csharpPINVOKE.IRVector_size(swigCPtr);
    return ret;
  }

  private uint capacity() {
    uint ret = lemur_csharpPINVOKE.IRVector_capacity(swigCPtr);
    return ret;
  }

  private void reserve(uint n) {
    lemur_csharpPINVOKE.IRVector_reserve(swigCPtr, n);
  }

  public IRVector() : this(lemur_csharpPINVOKE.new_IRVector__SWIG_0(), true) {
  }

  public IRVector(int capacity) : this(lemur_csharpPINVOKE.new_IRVector__SWIG_1(capacity), true) {
    if (lemur_csharpPINVOKE.SWIGPendingException.Pending) throw lemur_csharpPINVOKE.SWIGPendingException.Retrieve();
  }

  private IndexedReal getitemcopy(int index) {
    IndexedReal ret = new IndexedReal(lemur_csharpPINVOKE.IRVector_getitemcopy(swigCPtr, index), true);
    if (lemur_csharpPINVOKE.SWIGPendingException.Pending) throw lemur_csharpPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  private IndexedReal getitem(int index) {
    IndexedReal ret = new IndexedReal(lemur_csharpPINVOKE.IRVector_getitem(swigCPtr, index), false);
    if (lemur_csharpPINVOKE.SWIGPendingException.Pending) throw lemur_csharpPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  private void setitem(int index, IndexedReal value) {
    lemur_csharpPINVOKE.IRVector_setitem(swigCPtr, index, IndexedReal.getCPtr(value));
    if (lemur_csharpPINVOKE.SWIGPendingException.Pending) throw lemur_csharpPINVOKE.SWIGPendingException.Retrieve();
  }

  public void AddRange(IRVector values) {
    lemur_csharpPINVOKE.IRVector_AddRange(swigCPtr, IRVector.getCPtr(values));
    if (lemur_csharpPINVOKE.SWIGPendingException.Pending) throw lemur_csharpPINVOKE.SWIGPendingException.Retrieve();
  }

  public IRVector GetRange(int index, int count) {
    IntPtr cPtr = lemur_csharpPINVOKE.IRVector_GetRange(swigCPtr, index, count);
    IRVector ret = (cPtr == IntPtr.Zero) ? null : new IRVector(cPtr, true);
    if (lemur_csharpPINVOKE.SWIGPendingException.Pending) throw lemur_csharpPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public void Insert(int index, IndexedReal value) {
    lemur_csharpPINVOKE.IRVector_Insert(swigCPtr, index, IndexedReal.getCPtr(value));
    if (lemur_csharpPINVOKE.SWIGPendingException.Pending) throw lemur_csharpPINVOKE.SWIGPendingException.Retrieve();
  }

  public void InsertRange(int index, IRVector values) {
    lemur_csharpPINVOKE.IRVector_InsertRange(swigCPtr, index, IRVector.getCPtr(values));
    if (lemur_csharpPINVOKE.SWIGPendingException.Pending) throw lemur_csharpPINVOKE.SWIGPendingException.Retrieve();
  }

  public void RemoveAt(int index) {
    lemur_csharpPINVOKE.IRVector_RemoveAt(swigCPtr, index);
    if (lemur_csharpPINVOKE.SWIGPendingException.Pending) throw lemur_csharpPINVOKE.SWIGPendingException.Retrieve();
  }

  public void RemoveRange(int index, int count) {
    lemur_csharpPINVOKE.IRVector_RemoveRange(swigCPtr, index, count);
    if (lemur_csharpPINVOKE.SWIGPendingException.Pending) throw lemur_csharpPINVOKE.SWIGPendingException.Retrieve();
  }

  public static IRVector Repeat(IndexedReal value, int count) {
    IntPtr cPtr = lemur_csharpPINVOKE.IRVector_Repeat(IndexedReal.getCPtr(value), count);
    IRVector ret = (cPtr == IntPtr.Zero) ? null : new IRVector(cPtr, true);
    if (lemur_csharpPINVOKE.SWIGPendingException.Pending) throw lemur_csharpPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public void Reverse() {
    lemur_csharpPINVOKE.IRVector_Reverse__SWIG_0(swigCPtr);
  }

  public void Reverse(int index, int count) {
    lemur_csharpPINVOKE.IRVector_Reverse__SWIG_1(swigCPtr, index, count);
    if (lemur_csharpPINVOKE.SWIGPendingException.Pending) throw lemur_csharpPINVOKE.SWIGPendingException.Retrieve();
  }

  public void SetRange(int index, IRVector values) {
    lemur_csharpPINVOKE.IRVector_SetRange(swigCPtr, index, IRVector.getCPtr(values));
    if (lemur_csharpPINVOKE.SWIGPendingException.Pending) throw lemur_csharpPINVOKE.SWIGPendingException.Retrieve();
  }

}

}