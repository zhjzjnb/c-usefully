
DIR="$(cd $(dirname $0); pwd)"
cd ${DIR}
if [ $# != 3 ] ; then 
    echo "USAGE: ./go.sh dir sign key" 
    exit 1; 
fi 
sign=$2
key=$3

function decode_recursive() {
    for innerfile in $1/*
    do
        if [ -d "$innerfile" ]; then
            decode_recursive "$innerfile"
        elif [ -f "$innerfile" ]; then
            # echo "file:${innerfile}"
            ${DIR}/xxtea "$innerfile" "${sign}" "${key}"
        fi
    done
}
decode_recursive $1