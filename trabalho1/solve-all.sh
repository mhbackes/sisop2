# !usr/bin/env bash

###############################
####### NOT TESTED YET ########
###############################

HOME=`pwd`
TESTS_DIR=${HOME}/test
RESULTS=${HOME}/results

TIMEFORMAT=%R

numOfThreads='2
4
8'

if [ ! -d "${RESULTS}" ]; then
    mkdir ${RESULTS}
fi

echo size time > ${RESULTS}/results.log

for teste in ${TESTS_DIR}/*txt; do
    testName=`echo $teste | sed 's/.*\///'` #nome do arquivo teste 
    outputName=`basename $testName .txt`.sol #nome do arquivo de saída
    sizeMatrix=`basename $testName .txt | sed 's/x.*//'` #tamanho da matriz quadrada
    for thread in $numOfThreads; do
	    echo -n  $(( $sizeMatrix * $sizeMatrix )) $thread" " >> ${RESULTS}/results.log 
	for exec in `seq 1 10`; do
	    tempo=`(time ./matrix-thread $thread ${TESTS_DIR}/$testName ${TESTS_DIR}/$testName ${RESULTS}/$outputName) 2>&1`
	    echo -n $tempo " " >> ${RESULTS}/results.log 
	done
	echo " " >> ${RESULTS}/results.log 
    done
done
