# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    nm_tester.sh                                       :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/06/24 11:12:02 by plouvel           #+#    #+#              #
#    Updated: 2024/07/01 11:13:20 by plouvel          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

Black='\033[0;30m'        # Black
Red='\033[0;31m'          # Red
Green='\033[0;32m'        # Green
Yellow='\033[0;33m'       # Yellow
Blue='\033[0;34m'         # Blue
Purple='\033[0;35m'       # Purple
Cyan='\033[0;36m'         # Cyan
White='\033[0;37m'        # White
BBlack='\033[1;30m'       # Black
BRed='\033[1;31m'         # Red
BGreen='\033[1;32m'       # Green
BYellow='\033[1;33m'      # Yellow
BBlue='\033[1;34m'        # Blue
BPurple='\033[1;35m'      # Purple
BCyan='\033[1;36m'        # Cyan
BWhite='\033[1;37m'       # White
UBlack='\033[4;30m'       # Black
URed='\033[4;31m'         # Red
UGreen='\033[4;32m'       # Green
UYellow='\033[4;33m'      # Yellow
UBlue='\033[4;34m'        # Blue
UPurple='\033[4;35m'      # Purple
UCyan='\033[4;36m'        # Cyan
UWhite='\033[4;37m'       # White
On_Black='\033[40m'       # Black
On_Red='\033[41m'         # Red
On_Green='\033[42m'       # Green
On_Yellow='\033[43m'      # Yellow
On_Blue='\033[44m'        # Blue
On_Purple='\033[45m'      # Purple
On_Cyan='\033[46m'        # Cyan
On_White='\033[47m'       # White
IBlack='\033[0;90m'       # Black
IRed='\033[0;91m'         # Red
IGreen='\033[0;92m'       # Green
IYellow='\033[0;93m'      # Yellow
IBlue='\033[0;94m'        # Blue
IPurple='\033[0;95m'      # Purple
ICyan='\033[0;96m'        # Cyan
IWhite='\033[0;97m'       # White
BIBlack='\033[1;90m'      # Black
BIRed='\033[1;91m'        # Red
BIGreen='\033[1;92m'      # Green
BIYellow='\033[1;93m'     # Yellow
BIBlue='\033[1;94m'       # Blue
BIPurple='\033[1;95m'     # Purple
BICyan='\033[1;96m'       # Cyan
BIWhite='\033[1;97m'      # White
On_IBlack='\033[0;100m'   # Black
On_IRed='\033[0;101m'     # Red
On_IGreen='\033[0;102m'   # Green
On_IYellow='\033[0;103m'  # Yellow
On_IBlue='\033[0;104m'    # Blue
On_IPurple='\033[0;105m'  # Purple
On_ICyan='\033[0;106m'    # Cyan
On_IWhite='\033[0;107m'   # White
NC='\033[0m'



# CONFIGURATION

DIRS_TO_SCAN="/usr/bin"
YOUR_NM=./ft_nm
SYS_NM=nm
DIFF=diff
DIFF_SAVE_PATH="diffs"
OPTS="-uar"

SAVE_EVEN_IF_OK=1

# END OF CONFIGURATION

KO_COUNT=0
OK_COUNT=0

for DIR in $DIRS_TO_SCAN; do
    echo "Command launched to test your nm: ${BIWhite}$YOUR_NM $OPTS${NC}"
    echo "Command launched to test the system nm: ${BIWhite}$SYS_NM $OPTS${NC}\n"

    echo -n "Scanning ${BIWhite}$DIR${NC}... "

    ELF_EXECUTABLES=$(find $DIR -type f -executable -exec file {} \; | grep -i '.*elf.*not stripped' | awk -F ': ' '{print $1}')
    ELF_EXECUTABLES_NBR=$(echo $ELF_EXECUTABLES | wc -w)

    echo "${Green}found${NC} ${ELF_EXECUTABLES_NBR} ELF executables to test.\n"

    if [ -z "$ELF_EXECUTABLES" ]; then
        echo "No ELF executables found in ${BIWhite}$DIR${NC}... skipping."
        continue
    fi

    for EXEC in $ELF_EXECUTABLES; do
        TMP_A=$(mktemp)
        TMP_B=$(mktemp)
        TMP_DIFF=$(mktemp)

        $YOUR_NM $OPTS $EXEC > $TMP_A 2>/dev/null
        $SYS_NM $OPTS $EXEC > $TMP_B 2>/dev/null

        $DIFF $TMP_A $TMP_B > $TMP_DIFF

        if [ $? -ne 0 ] || [ $SAVE_EVEN_IF_OK -eq 1 ]; then
            mkdir -p $DIFF_SAVE_PATH

            EXEC_CLEAN=$(echo $EXEC | cut -c2-)

            mkdir -p $DIFF_SAVE_PATH/$EXEC_CLEAN

            cp $TMP_A $DIFF_SAVE_PATH/$EXEC_CLEAN/$YOUR_NM.output
            cp $TMP_B $DIFF_SAVE_PATH/$EXEC_CLEAN/$SYS_NM.output
            cp $TMP_DIFF $DIFF_SAVE_PATH/$EXEC_CLEAN/output.diff
        fi
        if [ $? -ne 0 ]; then
            echo "$EXEC : ${Red}KO${NC}."
            KO_COUNT=$((KO_COUNT + 1))
        else
            echo "$EXEC : ${Green}OK${NC}."
            OK_COUNT=$((OK_COUNT + 1))
        fi

        rm $TMP_A $TMP_B $TMP_DIFF
    done

    echo "\nResult for ${BIWhite}${DIR}${NC} : ${IGreen}$OK_COUNT${NC} OK, ${IRed}$KO_COUNT${NC} KO on ${BIWhite}${ELF_EXECUTABLES_NBR}${NC} files. You can find more details in the folder $DIFF_SAVE_PATH."

    OK_COUNT=0
    KO_COUNT=0
done
