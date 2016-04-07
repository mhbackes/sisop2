# !usr/bin/env bash

###############################
####### NOT TESTED YET ########
###############################

HOME=`pwd`
TESTS_DIR=${HOME}/test
RESULTS=${HOME}/results
RES_THREAD=${RESULTS}/thread
RES_FORK=${RESULTS}/fork

TIMEFORMAT=%R

numOfThreads='2
4
8'

if [ ! -d "${RESULTS}" ]; then
    mkdir ${RESULTS}
    mkdir ${RES_THREAD}
    mkdir ${RES_FORK}
fi

for teste in ${TESTS_DIR}/*txt; do
    testName=`echo $teste | sed 's/.*\///'` #nome do arquivo teste 
    outputName=`basename $testName .txt`.sol #nome do arquivo de saída
    sizeMatrix=`basename $testName .txt | sed 's/x.*//'` #tamanho da matriz quadrada
    for thread in $numOfThreads; do
	    echo -n  $(( $sizeMatrix * $sizeMatrix )) $thread" " >> ${RES_THREAD}/results.log 
	for exec in `seq 1 10`; do
	    tempo=`(time ./matrix-thread $thread ${TESTS_DIR}/$testName ${TESTS_DIR}/$testName ${RES_THREAD}/$outputName) 2>&1`
	    echo -n $tempo " " >> ${RES_THREAD}/results.log 
	done
	echo " " >> ${RES_THREAD}/results.log 
    done
done

for teste in ${TESTS_DIR}/*txt; do
    testName=`echo $teste | sed 's/.*\///'` #nome do arquivo teste 
    outputName=`basename $testName .txt`.sol #nome do arquivo de saída
    sizeMatrix=`basename $testName .txt | sed 's/x.*//'` #tamanho da matriz quadrada
    for thread in $numOfThreads; do
	    echo -n  $(( $sizeMatrix * $sizeMatrix )) $thread" " >> ${RES_FORK}/results.log 
	for exec in `seq 1 10`; do
	    tempo=`(time ./matrix-thread $thread ${TESTS_DIR}/$testName ${TESTS_DIR}/$testName ${RES_FORK}/$outputName) 2>&1`
	    echo -n $tempo " " >> ${RES_FORK}/results.log 
	done
	echo " " >> ${RES_FORK}/results.log 
    done
done
