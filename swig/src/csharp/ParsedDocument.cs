/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.0
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

namespace Indri {

using System;
using System.Runtime.InteropServices;

public class ParsedDocument : IDisposable {
  private HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal ParsedDocument(IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(ParsedDocument obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  ~ParsedDocument() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          indri_csharpPINVOKE.delete_ParsedDocument(swigCPtr);
        }
        swigCPtr = new HandleRef(null, IntPtr.Zero);
      }
      GC.SuppressFinalize(this);
    }
  }

  public string text {
    set {
      indri_csharpPINVOKE.ParsedDocument_text_set(swigCPtr, value);
    } 
    get {
      string ret = indri_csharpPINVOKE.ParsedDocument_text_get(swigCPtr);
      return ret;
    } 
  }

  public uint textLength {
    set {
      indri_csharpPINVOKE.ParsedDocument_textLength_set(swigCPtr, value);
    } 
    get {
      uint ret = indri_csharpPINVOKE.ParsedDocument_textLength_get(swigCPtr);
      return ret;
    } 
  }

  public string content {
    set {
      indri_csharpPINVOKE.ParsedDocument_content_set(swigCPtr, value);
    } 
    get {
      string ret = indri_csharpPINVOKE.ParsedDocument_content_get(swigCPtr);
      return ret;
    } 
  }

  public uint contentLength {
    set {
      indri_csharpPINVOKE.ParsedDocument_contentLength_set(swigCPtr, value);
    } 
    get {
      uint ret = indri_csharpPINVOKE.ParsedDocument_contentLength_get(swigCPtr);
      return ret;
    } 
  }

  public string getContent() {
    string ret = indri_csharpPINVOKE.ParsedDocument_getContent(swigCPtr);
    return ret;
  }

  public Char_pVector terms {
    set {
      indri_csharpPINVOKE.ParsedDocument_terms_set(swigCPtr, Char_pVector.getCPtr(value));
    } 
    get {
      IntPtr cPtr = indri_csharpPINVOKE.ParsedDocument_terms_get(swigCPtr);
      Char_pVector ret = (cPtr == IntPtr.Zero) ? null : new Char_pVector(cPtr, false);
      return ret;
    } 
  }

  public TagExtentVector tags {
    set {
      indri_csharpPINVOKE.ParsedDocument_tags_set(swigCPtr, TagExtentVector.getCPtr(value));
    } 
    get {
      IntPtr cPtr = indri_csharpPINVOKE.ParsedDocument_tags_get(swigCPtr);
      TagExtentVector ret = (cPtr == IntPtr.Zero) ? null : new TagExtentVector(cPtr, false);
      return ret;
    } 
  }

  public TermExtentVector positions {
    set {
      indri_csharpPINVOKE.ParsedDocument_positions_set(swigCPtr, TermExtentVector.getCPtr(value));
    } 
    get {
      IntPtr cPtr = indri_csharpPINVOKE.ParsedDocument_positions_get(swigCPtr);
      TermExtentVector ret = (cPtr == IntPtr.Zero) ? null : new TermExtentVector(cPtr, false);
      return ret;
    } 
  }

  public MetadataPairVector metadata {
    set {
      indri_csharpPINVOKE.ParsedDocument_metadata_set(swigCPtr, MetadataPairVector.getCPtr(value));
    } 
    get {
      IntPtr cPtr = indri_csharpPINVOKE.ParsedDocument_metadata_get(swigCPtr);
      MetadataPairVector ret = (cPtr == IntPtr.Zero) ? null : new MetadataPairVector(cPtr, false);
      return ret;
    } 
  }

  public ParsedDocument() : this(indri_csharpPINVOKE.new_ParsedDocument(), true) {
  }

}

}
