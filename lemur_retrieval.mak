# Microsoft Developer Studio Generated NMAKE File, Based on lemur_retrieval.dsp
!IF "$(CFG)" == ""
CFG=lemur_retrieval - Win32 Release
!MESSAGE No configuration specified. Defaulting to lemur_retrieval - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "lemur_retrieval - Win32 Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "lemur_retrieval.mak" CFG="lemur_retrieval - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "lemur_retrieval - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe
OUTDIR=.\retrieval\obj
INTDIR=.\retrieval\obj

ALL : ".\lemur_retrieval.lib"


CLEAN :
	-@erase "$(INTDIR)\CORIRetMethod.obj"
	-@erase "$(INTDIR)\CosSimRetMethod.obj"
	-@erase "$(INTDIR)\FreqVector.obj"
	-@erase "$(INTDIR)\IndexedReal.obj"
	-@erase "$(INTDIR)\InQueryRetMethod.obj"
	-@erase "$(INTDIR)\MatchInfo.obj"
	-@erase "$(INTDIR)\OkapiRetMethod.obj"
	-@erase "$(INTDIR)\ProxInfo.obj"
	-@erase "$(INTDIR)\QueryNode.obj"
	-@erase "$(INTDIR)\ResultFile.obj"
	-@erase "$(INTDIR)\RetMethodManager.obj"
	-@erase "$(INTDIR)\RetrievalMethod.obj"
	-@erase "$(INTDIR)\ScoreAccumulator.obj"
	-@erase "$(INTDIR)\SimpleKLRetMethod.obj"
	-@erase "$(INTDIR)\StringQuery.obj"
	-@erase "$(INTDIR)\StructQueryRep.obj"
	-@erase "$(INTDIR)\StructQueryRetMethod.obj"
	-@erase "$(INTDIR)\TextQueryRep.obj"
	-@erase "$(INTDIR)\TextQueryRetMethod.obj"
	-@erase "$(INTDIR)\TFIDFRetMethod.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\XLingRetMethod.obj"
	-@erase ".\lemur_retrieval.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\lemur_retrieval.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"lemur_retrieval.lib" 
LIB32_OBJS= \
	"$(INTDIR)\CORIRetMethod.obj" \
	"$(INTDIR)\CosSimRetMethod.obj" \
	"$(INTDIR)\FreqVector.obj" \
	"$(INTDIR)\IndexedReal.obj" \
	"$(INTDIR)\InQueryRetMethod.obj" \
	"$(INTDIR)\MatchInfo.obj" \
	"$(INTDIR)\OkapiRetMethod.obj" \
	"$(INTDIR)\ProxInfo.obj" \
	"$(INTDIR)\QueryNode.obj" \
	"$(INTDIR)\ResultFile.obj" \
	"$(INTDIR)\RetMethodManager.obj" \
	"$(INTDIR)\RetrievalMethod.obj" \
	"$(INTDIR)\ScoreAccumulator.obj" \
	"$(INTDIR)\SimpleKLRetMethod.obj" \
	"$(INTDIR)\StringQuery.obj" \
	"$(INTDIR)\StructQueryRep.obj" \
	"$(INTDIR)\StructQueryRetMethod.obj" \
	"$(INTDIR)\TextQueryRep.obj" \
	"$(INTDIR)\TextQueryRetMethod.obj" \
	"$(INTDIR)\TFIDFRetMethod.obj" \
	"$(INTDIR)\XLingRetMethod.obj"

".\lemur_retrieval.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

CPP_PROJ=/nologo /ML /W3 /GR /GX /O2 /I "utility\include" /I "index\include" /I "langmod\include" /I "retrieval\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\lemur_retrieval.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("lemur_retrieval.dep")
!INCLUDE "lemur_retrieval.dep"
!ELSE 
!MESSAGE Warning: cannot find "lemur_retrieval.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "lemur_retrieval - Win32 Release"
SOURCE=.\retrieval\src\CORIRetMethod.cpp

"$(INTDIR)\CORIRetMethod.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\retrieval\src\CosSimRetMethod.cpp

"$(INTDIR)\CosSimRetMethod.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\retrieval\src\FreqVector.cpp

"$(INTDIR)\FreqVector.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\retrieval\src\IndexedReal.cpp

"$(INTDIR)\IndexedReal.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\retrieval\src\InQueryRetMethod.cpp

"$(INTDIR)\InQueryRetMethod.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\retrieval\src\MatchInfo.cpp

"$(INTDIR)\MatchInfo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\retrieval\src\OkapiRetMethod.cpp

"$(INTDIR)\OkapiRetMethod.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\retrieval\src\ProxInfo.cpp

"$(INTDIR)\ProxInfo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\retrieval\src\QueryNode.cpp

"$(INTDIR)\QueryNode.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\retrieval\src\ResultFile.cpp

"$(INTDIR)\ResultFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\retrieval\src\RetMethodManager.cpp

"$(INTDIR)\RetMethodManager.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\retrieval\src\RetrievalMethod.cpp

"$(INTDIR)\RetrievalMethod.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\retrieval\src\ScoreAccumulator.cpp

"$(INTDIR)\ScoreAccumulator.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\retrieval\src\SimpleKLRetMethod.cpp

"$(INTDIR)\SimpleKLRetMethod.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\retrieval\src\StringQuery.cpp

"$(INTDIR)\StringQuery.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\retrieval\src\StructQueryRep.cpp

"$(INTDIR)\StructQueryRep.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\retrieval\src\StructQueryRetMethod.cpp

"$(INTDIR)\StructQueryRetMethod.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\retrieval\src\TextQueryRep.cpp

"$(INTDIR)\TextQueryRep.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\retrieval\src\TextQueryRetMethod.cpp

"$(INTDIR)\TextQueryRetMethod.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\retrieval\src\TFIDFRetMethod.cpp

"$(INTDIR)\TFIDFRetMethod.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\retrieval\src\XLingRetMethod.cpp

"$(INTDIR)\XLingRetMethod.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

