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

public class lemur_csharp {
  public static void ParamSet(string key, string value) {
    lemur_csharpPINVOKE.ParamSet(key, value);
    if (lemur_csharpPINVOKE.SWIGPendingException.Pending) throw lemur_csharpPINVOKE.SWIGPendingException.Retrieve();
  }

  public static void ParamClear() {
    lemur_csharpPINVOKE.ParamClear();
    if (lemur_csharpPINVOKE.SWIGPendingException.Pending) throw lemur_csharpPINVOKE.SWIGPendingException.Retrieve();
  }

  public static int ParamPushFile(string s) {
    int ret = lemur_csharpPINVOKE.ParamPushFile(s);
    if (lemur_csharpPINVOKE.SWIGPendingException.Pending) throw lemur_csharpPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static string ParamPopFile() {
    string ret = lemur_csharpPINVOKE.ParamPopFile();
    if (lemur_csharpPINVOKE.SWIGPendingException.Pending) throw lemur_csharpPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

}

}