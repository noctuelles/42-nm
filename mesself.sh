# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    mesself.sh                                         :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/06/21 20:20:49 by plouvel           #+#    #+#              #
#    Updated: 2024/06/21 20:44:26 by plouvel          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#!/bin/bash


# CONFIGURATION

FT_NM="ft_nm"
CRASH_DIR="crash_file"
SEGV=11

# END OF CONFIGURATION

MESS_HDR=0
MESS_SYMS=0
MESS_SECTIONS=0

FILE=""

show_help() {
    echo "Mess with an ELF file to make your $FT_NM"
    echo "This script will randomly change a byte in a choosen location of the ELF file, and run $FT_NM on it."
    echo "If $FT_NM crashes, the file will be saved in a $CRASH_DIR directory."
    echo "Usage: $0 [-h] [-s] -f <file>"
    echo "  -h: mess with the ELF header"
    echo "  -j: mess with the section headers"
    echo "  -s: mess with the symbols table"
    echo "  -v: verbose"
    echo "  -f: file to mess with"
}

if [ $# -eq 0 ]; then
    show_help
    exit 1
fi

while getopts "hsf:j" opt; do
  case "$opt" in
    h) MESS_HDR=1;;
    s) MESS_SYMS=1;;
    j) MESS_SECTIONS=1;;
    f) FILE=$OPTARG;;
    \?) exit 1;;
    :) exit 1;;
  esac
done

if ! [ -f $FILE ]; then
    echo "File $FILE does not exist."
    exit 1
fi

if ! [ -x $FILE ]; then
    echo "File $FILE is not an executable."
    exit 1
fi

HSIZE=$(readelf -h $FILE | grep "Size of this header" | awk '{print $5}')
SHOFF=$(readelf -h $FILE | grep "Start of section headers" | awk '{print $5}')
SHENTSIZE=$(readelf -h $FILE | grep "Size of section headers" | awk '{print $5}')
SHNUM=$(readelf -h $FILE | grep "Number of section headers" | awk '{print $5}')

SYMTAB_NDX=$(readelf -S $FILE | grep -A1 "SYMTAB" | awk 'NR==1 {print $1}' | sed 's/[^0-9]//g')

SYMTAB_OFF=$(printf "%d" 0x$(readelf -S $FILE | grep -A1 "SYMTAB" | awk 'NR==1 {print $5}'))
SYMTAB_SIZE=$(printf "%d" 0x$(readelf -S $FILE | grep -A1 "SYMTAB" | awk 'NR==2 {print $1}'))

echo $SYMTAB_OFF
echo $SYMTAB_SIZE

while true; do
    if [ $MESS_HDR -eq 1 ]; then
        RANDOM_OFFSET=$(shuf -i 0-$HSIZE -n 1)
    elif [ $MESS_SYMS -eq 1 ]; then
        RANDOM_OFFSET=$(shuf -i $SYMTAB_OFF-$(($SYMTAB_OFF + $SYMTAB_SIZE)) -n 1)
    elif [ $MESS_SECTIONS -eq 1 ]; then
        RANDOM_OFFSET=$(shuf -i $SHOFF-$(($SHOFF + $SHENTSIZE * $SHNUM)) -n 1)
    else
        echo "You must specify where to mess with the ELF."
        exit 1
    fi
    RANDOM_BYTE_VALUE=$(shuf -i 0-255 -n 1)

    TMP=$(mktemp)
    cp $FILE $TMP

    printf "\x$(printf %x $RANDOM_BYTE_VALUE)" | dd of=$TMP bs=1 seek=$RANDOM_OFFSET count=1 conv=notrunc > /dev/null 2>&1

    echo $TMP

    ./ft_nm $TMP > /dev/null

    if [ $? -eq $((128 + $SEGV)) ]; then
        mkdir -p ./crash_file/

        CRASH_DUMP_FILE=./crash_file/${FILE}_${RANDOM_OFFSET}_${RANDOM_BYTE_VALUE}

        cp $TMP $CRASH_DUMP_FILE
        echo "Program received Segmentation Fault, saving file to $CRASH_DUMP_FILE ..."
        continue
    fi

    rm -f $TMP
done