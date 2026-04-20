#!/bin/sh

# SPDX-FileCopyrightText: 2008 Thomas Günther <tom@toms-cafe.de>
#
# SPDX-License-Identifier: GPL-1.0-or-later OR Artistic-2.0

set -e

# Download sudokus from sudokugarden.de
URL="http://sudokugarden.de/de/online/"
YAS="./yasss"

# List of sudokus with 17 givens from http://www.csse.uwa.edu.au/~gordon/sudokumin.php
# (retrieved from https://web.archive.org/web/20110414052628/http://mapleta.maths.uwa.edu.au/~gordon/sudokumin.php)
# (last version of sudoku17 with 49151 sudokus, in web.archive.org since 2011-04-14, available until 2020_07_26)
S17_ORIGINAL="sudoku17"
S17_CANONICAL="sudoku17C"

# Result file with downloaded sudokus
LOG="sudokugarden.log"
TMP=$(mktemp)

if ! [ -x "${YAS}" ]; then
    echo "${YAS} is missing or not executable"
    echo "- Download sources from https://github.com/moritz/yasss.git"
    echo "- Compile sources with 'make'"
    echo "- Copy program to this directory"
    exit 1
fi

if ! [ -e "${S17_CANONICAL}" ]; then
    echo "${S17_CANONICAL} is missing"
    echo "Converting ${S17_ORIGINAL} to ${S17_CANONICAL} ..."

    # Write header to list
    printf "Rating   %-89s   %s\n" "Sudoku" "Canonical_form" > "${S17_CANONICAL}"

    # Convert each sudoku line by line
    while read SUD; do
        RAT=$(echo "${SUD}" | "${YAS}" -s | sed -e 's/Score: //')
        RAT=$(printf "%6d" "${RAT}")

        S17=$(echo "${SUD}" | sed -e 's/0/_/g;s/\(.........\)/\1+/g;s/+$//')

        CAN=$(echo "${SUD}" | "${YAS}" -C)
        CAN=$(echo "${CAN}" | sed -e 's/0/_/g;s/\(.........\)/\1+/g;s/+$//')

        echo "${RAT}   ${S17}   ${CAN}" >> "${S17_CANONICAL}"
        echo -n "."
    done < "${S17_ORIGINAL}"
    echo "\nConverted ${S17_ORIGINAL} to ${S17_CANONICAL}"
fi

if ! [ -e "${LOG}" ]; then
    # Write header to log file
    printf "Position   Rating   %-89s   %-89s   %-89s   Cmp    Date\n" \
        "Sudoku_in_suduku17C_list" "Canonical_form" "Sudoku_from_sudokugarden.de" \
        > "${LOG}"
fi

DAT=$(tail -n1 "${LOG}" 2>/dev/null | sed -e 's/.* //')
if (echo "${DAT}" | grep -q "[0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9]"); then
    wget -q -O "${TMP}" "${URL}${DAT}"
    DAT=$(sed -ne '/chster Tag/s/^.*href="\/de\/online\/\([^"]*\)".*$/\1/p' "${TMP}")
else
    DAT="2006-06-25"
fi

# Download webpages from sudokugarden.de, extract sudoku and search in suduku17C list
while [ "${DAT}" ]; do

    # Download webpage
    wget -q -O "${TMP}" "${URL}${DAT}"

    # Extract sudoku
    OUT=$(sed -ne '/value=".*\/input>/s/^.*value="\(.\).*$/\1/p;/<\/tr>/s/^.*$/+/p' "${TMP}")
    SUD=$(echo ${OUT} | sed -e 's/ //g;s/"/_/g;s/^+*//;s/+*$//')

    # Convert sudoku to canonical form
    CAN=$(echo "${SUD}" | sed -e 's/_/0/g;s/+//g' | "${YAS}" -C)
    CAN=$(echo "${CAN}" | sed -e 's/0/_/g;s/\(.........\)/\1+/g;s/+$//')

    # Search canonical sudoku in sudoku17C list
    POS_RAT_S17=$(grep -n "${CAN}" "${S17_CANONICAL}" | sed -e 's/:/ /')
    if [ "${POS_RAT_S17}" ]; then
        set ${POS_RAT_S17}
        POS=$1; RAT=$2; S17=$3
        POS=$(printf "%8d" $((${POS} - 1)))
        RAT=$(printf "%6d" "${RAT}")
        CMP=$(test "${SUD}" = "${S17}" && echo "ORIG" || echo "PERM")
    else
        POS="NotFound"
        RAT="     -"
        S17=$(printf "%-89s" "-")
        CMP="----"
    fi

    # Write result to log file
    echo "${POS}   ${RAT}   ${S17}   ${CAN}   ${SUD}   ${CMP}   ${DAT}" | tee -a "${LOG}"

    # Extract url of next webpage
    DAT=$(sed -ne '/chster Tag/s/^.*href="\/de\/online\/\([^"]*\)".*$/\1/p' "${TMP}")

    rm "${TMP}"
done

exit


# Find doubles in sudokugarden.log
# grep -v '^NotFound' sudokugarden.log | sort -n | uniq --all-repeated=prepend -w8

# Copy sudokus from sudokugarden.log to sudoku-list
# grep -v '^Position' sudokugarden.log | sed -e 's/^ *[^ ]*   \( *[^ ]*\) *[^ ]* *[^ ]* *\([^ ]*\) *[^ ]* *\([^ ]*\)$/\2 sudokugarden.de \3 (Score: \1)/' >> ~/.sudoku/sudoku-list

# Search for not used sudokus
# grep -v '^Position' sudokugarden.log | cut -c 1-8 | sort -n | uniq > sudokugarden.POS
# POS=1; while [ ${POS} -le 49151 ]; do printf "%8d\n" ${POS}; POS=$((${POS} + 1)); done | diff -u - sudokugarden.POS

# Count ratings in sudoku17C
# grep -v '^Rating' sudoku17C | cut -c 1-6 | sort -n | uniq -c

# Compare canonical sudokus from "yasss -C" with sudoku17-ml from http://www.csse.uwa.edu.au/~gordon/sudokumin.php
# wget --max-redirect=2 https://web.archive.org/web/20110414052628/http://mapleta.maths.uwa.edu.au/~gordon/sudoku17-ml
# cat sudoku17 | ./yasss -C | diff -u sudoku17-ml -
# cut -c 102- sudoku17C | grep -v Canonical_form | sed -e 's/_/0/g;s/\+//g' | diff -u sudoku17-ml -
