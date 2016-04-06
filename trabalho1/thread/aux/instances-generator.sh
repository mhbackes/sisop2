# !usr/bin/env bash




if [ $# -ne 3 ]; then
    echo "Uso correto:"
    echo "     ./instances-generator.sh <tamanho-inicial> <incremento> <tamanho-final>"
    echo "Só gera matrizes quadradas."
    exit 1
fi


for i in `seq $1 $2 $3`; do
    python3 matrix-generate.py $i $i > ../test/${i}x${i}.txt
done
