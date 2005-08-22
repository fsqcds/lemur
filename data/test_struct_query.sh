#!/bin/sh
## Edit this to point to your version of trec_eval
## if not found, it will be set to echo
TREC_EVAL=/usr/mel0/tmp1/dfisher/trec_eval/trec_eval
if test ! -x $TREC_EVAL ; then
  TREC_EVAL="echo not found: $TREC_EVAL"
fi


#######################################################################
# THIS IS THE TEST SCRIPT FOR STRUCTURED QUERIES                      #
#######################################################################

# First erasing any existing files 
./clean.sh

# Then prepare parameters
cp structparam/*_param .


#######################################################################
# The following command converts the queries from the structured      #
# query format to the basic lemur format  (output file: struct_query) #
#######################################################################

../app/obj/ParseInQueryOp parse_struct_query_param query.struct

#######################################################################
# The following command builds a position index of the CACM database  #
#  directly from a simple SGML format source                          #
# Structured queries require the use of a position index.             #
#######################################################################

../app/obj/BuildIndex build_param database.sgml


#######################################################################
# The following shows how to run simple and feedback retrieval        #
# experiments with structured queries.                                #
#######################################################################

# simple structured query retrieval example
../app/obj/StructQueryEval simple_struct_param
$TREC_EVAL -o -q qrel res.simple_struct  > pr.simple_struct

# simple structured query retrieval with pseudofeedback example
../app/obj/StructQueryEval fb_struct_param
$TREC_EVAL -o -q qrel res.fb_struct  > pr.fb_struct




