/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.36
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

namespace Lemur {

using System;
using System.Runtime.InteropServices;

public class RetrievalMethod : IDisposable {
  private HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal RetrievalMethod(IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(RetrievalMethod obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  ~RetrievalMethod() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if(swigCPtr.Handle != IntPtr.Zero && swigCMemOwn) {
        swigCMemOwn = false;
        lemur_csharpPINVOKE.delete_RetrievalMethod(swigCPtr);
      }
      swigCPtr = new HandleRef(null, IntPtr.Zero);
      GC.SuppressFinalize(this);
    }
  }

  public virtual void scoreCollection(QueryRep qry, IndexedRealVector results) {
    lemur_csharpPINVOKE.RetrievalMethod_scoreCollection(swigCPtr, QueryRep.getCPtr(qry), IndexedRealVector.getCPtr(results));
    if (lemur_csharpPINVOKE.SWIGPendingException.Pending) throw lemur_csharpPINVOKE.SWIGPendingException.Retrieve();
  }

  public virtual void updateQuery(QueryRep qryRep, WeightedIDSet relDocs) {
    lemur_csharpPINVOKE.RetrievalMethod_updateQuery(swigCPtr, QueryRep.getCPtr(qryRep), WeightedIDSet.getCPtr(relDocs));
    if (lemur_csharpPINVOKE.SWIGPendingException.Pending) throw lemur_csharpPINVOKE.SWIGPendingException.Retrieve();
  }

}

}
