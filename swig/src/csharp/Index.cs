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

public class Index : IDisposable {
  private HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal Index(IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(Index obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  public virtual void Dispose() {
    if(swigCPtr.Handle != IntPtr.Zero && swigCMemOwn) {
      swigCMemOwn = false;
      throw new MethodAccessException("C++ destructor does not have public access");
    }
    swigCPtr = new HandleRef(null, IntPtr.Zero);
    GC.SuppressFinalize(this);
  }

  public bool open(string indexName) {
    bool ret = lemur_csharpPINVOKE.Index_open(swigCPtr, indexName);
    if (lemur_csharpPINVOKE.SWIGPendingException.Pending) throw lemur_csharpPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int term(string word) {
    int ret = lemur_csharpPINVOKE.Index_term__SWIG_0(swigCPtr, word);
    if (lemur_csharpPINVOKE.SWIGPendingException.Pending) throw lemur_csharpPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public string term(int termID) {
    string ret = lemur_csharpPINVOKE.Index_term__SWIG_1(swigCPtr, termID);
    if (lemur_csharpPINVOKE.SWIGPendingException.Pending) throw lemur_csharpPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int document(string docIDStr) {
    int ret = lemur_csharpPINVOKE.Index_document__SWIG_0(swigCPtr, docIDStr);
    if (lemur_csharpPINVOKE.SWIGPendingException.Pending) throw lemur_csharpPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public string document(int docID) {
    string ret = lemur_csharpPINVOKE.Index_document__SWIG_1(swigCPtr, docID);
    if (lemur_csharpPINVOKE.SWIGPendingException.Pending) throw lemur_csharpPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public DocumentManager docManager(int docID) {
    IntPtr cPtr = lemur_csharpPINVOKE.Index_docManager(swigCPtr, docID);
    DocumentManager ret = (cPtr == IntPtr.Zero) ? null : new DocumentManager(cPtr, false);
    if (lemur_csharpPINVOKE.SWIGPendingException.Pending) throw lemur_csharpPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int docCount() {
    int ret = lemur_csharpPINVOKE.Index_docCount__SWIG_0(swigCPtr);
    if (lemur_csharpPINVOKE.SWIGPendingException.Pending) throw lemur_csharpPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int termCountUnique() {
    int ret = lemur_csharpPINVOKE.Index_termCountUnique(swigCPtr);
    if (lemur_csharpPINVOKE.SWIGPendingException.Pending) throw lemur_csharpPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int termCount(int termID) {
    int ret = lemur_csharpPINVOKE.Index_termCount__SWIG_0(swigCPtr, termID);
    if (lemur_csharpPINVOKE.SWIGPendingException.Pending) throw lemur_csharpPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int termCount() {
    int ret = lemur_csharpPINVOKE.Index_termCount__SWIG_1(swigCPtr);
    if (lemur_csharpPINVOKE.SWIGPendingException.Pending) throw lemur_csharpPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public float docLengthAvg() {
    float ret = lemur_csharpPINVOKE.Index_docLengthAvg(swigCPtr);
    if (lemur_csharpPINVOKE.SWIGPendingException.Pending) throw lemur_csharpPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int docCount(int termID) {
    int ret = lemur_csharpPINVOKE.Index_docCount__SWIG_1(swigCPtr, termID);
    if (lemur_csharpPINVOKE.SWIGPendingException.Pending) throw lemur_csharpPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int docLength(int docID) {
    int ret = lemur_csharpPINVOKE.Index_docLength(swigCPtr, docID);
    if (lemur_csharpPINVOKE.SWIGPendingException.Pending) throw lemur_csharpPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public DocInfoList docInfoList(int termID) {
    IntPtr cPtr = lemur_csharpPINVOKE.Index_docInfoList(swigCPtr, termID);
    DocInfoList ret = (cPtr == IntPtr.Zero) ? null : new DocInfoList(cPtr, true);
    if (lemur_csharpPINVOKE.SWIGPendingException.Pending) throw lemur_csharpPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public TermInfoList termInfoList(int docID) {
    IntPtr cPtr = lemur_csharpPINVOKE.Index_termInfoList(swigCPtr, docID);
    TermInfoList ret = (cPtr == IntPtr.Zero) ? null : new TermInfoList(cPtr, true);
    if (lemur_csharpPINVOKE.SWIGPendingException.Pending) throw lemur_csharpPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public TermInfoList termInfoListSeq(int docID) {
    IntPtr cPtr = lemur_csharpPINVOKE.Index_termInfoListSeq(swigCPtr, docID);
    TermInfoList ret = (cPtr == IntPtr.Zero) ? null : new TermInfoList(cPtr, true);
    if (lemur_csharpPINVOKE.SWIGPendingException.Pending) throw lemur_csharpPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public void setProps() {
    lemur_csharpPINVOKE.Index_setProps(swigCPtr);
  }

}

}