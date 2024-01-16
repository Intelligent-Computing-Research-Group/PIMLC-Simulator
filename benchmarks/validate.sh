# -b benchmarks name
# -r generate a specified number of random test datasets
# -i specify the input data
# input form:x0,x1,x2,...,xn
# simoutput
# vrltoutput

# Read testbench instructions

benchmarks=all
testdata=random.dat
random=10

# if [ $# -gt 1 ]; then
#     testdata="$2"
# fi
# echo $testdata

while getopts ":b:r:i:" opt; do
    case $opt in
    b)
        benchmarks=$OPTARG
        ;;
    r)
        random=$OPTARG
        ;;
    i)
        testdata=$OPTARG
        random=0
        ;;
    :)
        echo "Option -$OPTARG needs a parameter"
        exit 1
        ;;
    ?)
        echo "Invalid option -$OPTARG"
        exit 2
        ;;
    esac
done

if [ "$benchmarks" == "all" ]; then
    benchmarks=`ls *.asm 2>/dev/null`
    benchmarks=${benchmarks//.asm/}
fi
declare -a benchmarks

g++ genrand.cpp -o genrand
g++ tbdat.cpp -o tbdat
g++ ../pimlcsim.cpp ../simulator.cpp -o sim
for bm in ${benchmarks[@]}; do
    # generate result report
    if [ $random -ne 0 ]; then
        rm -f $testdata
        touch $testdata
        ./genrand $bm.asm $random >> $testdata
    fi
    rm -f ${bm}_rst.txt
    rm -rf obj_dir
    touch ${bm}_rst.txt


    # input data report
    echo "=====================================" >> ${bm}_rst.txt
    echo "Test Input Data(x0,x1,...):" >> ${bm}_rst.txt
    cat $testdata >> ${bm}_rst.txt
    echo "" >> ${bm}_rst.txt
    echo "" >> ${bm}_rst.txt


    # simulator result report
    echo "=====================================" >> ${bm}_rst.txt
    echo "Simulator Output(y0,y1,...):" >> ${bm}_rst.txt
    rm -f sim_${bm}_rst.txt
    ./sim $bm.asm $testdata > sim_${bm}_rst.txt
    cat sim_${bm}_rst.txt >> ${bm}_rst.txt
    echo "" >> ${bm}_rst.txt
    echo "" >> ${bm}_rst.txt


    # verilator result report
    echo "=====================================" >> ${bm}_rst.txt
    echo "Verilator Output(y0,y1,...):" >> ${bm}_rst.txt
    iodat=`./tbdat $bm.asm`
    ./genvtb.sh $bm $iodat
    verilator --cc $bm.v
    verilator -Wall --trace --cc ${bm}.v --exe tb_${bm}.cpp
    make -j32 -C obj_dir/ -f V${bm}.mk V${bm}
    rm -f vrlt_${bm}_rst.txt
    ./obj_dir/V$bm $testdata $iodat > vrlt_${bm}_rst.txt
    cat vrlt_${bm}_rst.txt >> ${bm}_rst.txt
    echo "" >> ${bm}_rst.txt
    echo "" >> ${bm}_rst.txt


    # compare
    echo "=====================================" >> ${bm}_rst.txt
    echo "Compare result:" >> ${bm}_rst.txt
    cmp sim_${bm}_rst.txt vrlt_${bm}_rst.txt >> ${bm}_rst.txt


    rm -rf obj_dir
    # rm sim_${bm}_rst.txt
    # rm vrlt_${bm}_rst.txt
    if [ $random -ne 0 ]; then
        rm $testdata
    fi
done
rm sim
rm genrand
rm tbdat
# if [ $random -ne 0 ]; then
#     # Use random test data
    
# fi

