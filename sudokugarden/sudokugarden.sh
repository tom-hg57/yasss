#!/bin/sh
set -e

# Download sudokus from sudokugarden.de
URL="http://sudokugarden.de/de/online/"
YAS="./yasss"

# List of sudokus with 17 givens from http://www.csse.uwa.edu.au/~gordon/sudokumin.php (2006-11-26)
S17_ORIGINAL="sudoku17"
S17_CANONICAL="sudoku17C"

# Result file with downloaded sudokus
LOG="sudokugarden.log"
TMP=$(mktemp)

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
    set $(grep -n "${CAN}" "${S17_CANONICAL}" | sed -e 's/:/ /')
    POS=$1; RAT=$2; S17=$3
    if [ "${POS}" ]; then
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
