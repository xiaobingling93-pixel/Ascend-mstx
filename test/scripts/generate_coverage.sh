#!/bin/bash
echo "***************Generate Coverage*****************"
CUR_DIR=$(dirname $(readlink -f $0))
TOP_DIR=${CUR_DIR}/..
cd ${TOP_DIR}/build_ut_c

if [ -d "./coverage" ]; then
    rm -rf ./coverage
fi
mkdir coverage

lcov -c -d ./test/test/c_cpp/CMakeFiles/mstx_static_lib_C.dir/ -o ./coverage/mstx_C.info -b ./coverage --rc lcov_branch_coverage=1
lcov -c -d ./test/test/c_cpp/CMakeFiles/mstx_static_lib_CPP.dir/ -o ./coverage/mstx_CPP.info -b ./coverage --rc lcov_branch_coverage=1

lcov -r ./coverage/mstx_C.info '*/thirdparty/*' -o ./coverage/mstx_C.info --rc lcov_branch_coverage=1
lcov -r ./coverage/mstx_C.info '*/test/*' -o ./coverage/mstx_C.info --rc lcov_branch_coverage=1
lcov -r ./coverage/mstx_C.info '*/c++/*' -o ./coverage/mstx_C.info --rc lcov_branch_coverage=1
lcov -r ./coverage/mstx_CPP.info '*/thirdparty/*' -o ./coverage/mstx_CPP.info --rc lcov_branch_coverage=1
lcov -r ./coverage/mstx_CPP.info '*/test/*' -o ./coverage/mstx_CPP.info --rc lcov_branch_coverage=1
lcov -r ./coverage/mstx_CPP.info '*/c++/*' -o ./coverage/mstx_CPP.info --rc lcov_branch_coverage=1

genhtml ./coverage/mstx_C.info ./coverage/mstx_CPP.info -o ./coverage/report --branch-coverage

cd coverage
tar -zcvf report.tar.gz ./report
