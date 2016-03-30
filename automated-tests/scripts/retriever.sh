#!/bin/bash
#
# Copyright (c) 2014 Samsung Electronics Co., Ltd.
#
# Licensed under the Apache License, Version 2.0 (the License);
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http:#www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
set -x;
USAGE=$(cat <<EOF
Usage note: retriever.sh [option] [directory]
Options:
  none    retrieve TC names with corresponding startup and cleanup functions
  -f      retrieveve TC name with corresponding "set" and "purpose" clauses
  -anum   retrieve automatic TC number
  -mnum   retrieve manual TC number

In case of TC in form of "int tc_name()" script will abort.
("int tc_name(void)" is a proper function signature)
EOF
)

function get_tc_files {
    CMAKE_FILE="$DIR/CMakeLists.txt"
    if [ ! -e $CMAKE_FILE ]; then
        echo "File $CMAKE_FILE for $DIR not found. Aborting..."
        exit 1
    fi

    TC_FILES=$(cat $CMAKE_FILE | awk -vDIR="$DIR" '
    BEGIN {
        flag = 0;
        files = "";
    }
    /^SET\(TC_SOURCES/ {
        flag = 1;
        next;
    }
    /\)/ {
        if (flag == 1)
            exit;
    }
    !/^ *#/ {
        if (flag == 1) {
            if (files == "")
                files = DIR "/" $1;
            else
                files = files " " DIR "/" $1;
        }
    }
    END {
        print files;
    }')
}

function tc_names {
    if [[ -z "$1" ]]; then
        exit
    fi

    awk '
    BEGIN {
        OFS = ",";
        start_fun = "NULL";
        clean_fun = "NULL";
        err_flag = 0;
        tc_list = "";
        curr_file = "";
    }
    {
        if(FILENAME != curr_file) {
            start_fun = "NULL";
            clean_fun = "NULL";
            curr_file = FILENAME;
        }
    }
    /^void .*startup\(void\)/ {
        gsub(/^void /, "");
        gsub(/\(void\)$/,"");
        start_fun = $0
    }
    /^void .*cleanup\(void\)/ {
        gsub(/^void /, "");
        gsub(/\(void\)$/,"");
        clean_fun = $0
    }
    /^int .*\(\)/ {
        print "Warning: function with empty argument list -- \"" $0 "\" in " FILENAME ":" FNR;
        err_flag = 1;
    }
    /^int .*\(void\)/ {
        gsub(/^int /, "");
        gsub(/\(void\).*/,"");
        if (tc_list != "") tc_list = tc_list "\n";
        tc_list = tc_list $0 OFS start_fun OFS clean_fun
    }
    END {
        if (err_flag) {
            exit 1
        } else {
            print tc_list
        }
    }
    ' $*
}

TFILE="/tmp/tnum.csv"
SCRIPT_DIR="$(cd "$(dirname $0)" && pwd)"
function tc_anum2 {
        if [ -f /tmp/temp2 ];then
			rm /tmp/temp2
		fi
        tc_fullinfo $TC_FILES > ${TFILE}_pre

		cat ${TFILE}_pre | sort -t',' -k2,2 -s > ${TFILE}_remove
		awk '!x[$0]++' ${TFILE}_remove > $TFILE

        for fname in `cat $SCRIPT_DIR/../exclude.list`
        do
           sed -i "/$fname/d" $TFILE
        done

	count=$(cat $TFILE |wc -l)
        echo $count

        rm $TFILE ${TFILE}_pre ${TFILE}_remove
}

function tc_anum {
    awk '
    BEGIN {
        count = 0;
        err_flag = 0;
    }
    /^int .*\(\)/ {
        print "Warning: function with empty argument list -- \"" $0 "\" in " FILENAME ":" FNR;
        err_flag = 1;
    }
    /^int .*\(void\)$/ {
        count++;
    }
    END {
        if (err_flag) {
            exit 1
        } else {
            print count
        }
    }
    ' $*
}

function tc_mnum {
    # TODO: fix this hardcoded value
    echo 0
}

function tc_fullinfo {
    awk '
    BEGIN {
        OFS=",";
        purpose = "";
        set = "Default";
        err_flag = 0;
        tc_list = "";
    }
    /^\/\/& purpose:/ {
        purpose = $3;
        for (i = 4; i <= NF; i++) {
            purpose = purpose " " $i;
        }
        next;
    }
    /^int .*\(\)/ {
        print "Warning: function with empty argument list -- \"" $0 "\" in " FILENAME ":" FNR;
        err_flag = 1;
    }
    /^int .*\(void\)$/ {
        gsub(/^int /, "");
        gsub(/\(void\)$/,"");
        if (tc_list != "") tc_list = tc_list "\n";
        tc_list = tc_list $0 OFS set OFS purpose;
        purpose = "";
        next
    }
    END {
        if (err_flag) {
            exit 1
        } else {
            print tc_list
        }
    }
    ' $*
}


# usage note and exit:
# - argument begin with '-' but is not recognised or number of arguments is > 2,
# - argument doesn't begin with '-' and number of arguments is > 1
if [[ ( "$1" == -* && ( ! "$1" =~ ^-(anum|mnum|f)$ || $# > 2 ) ) || ( "$1" != -* && $# > 1 ) ]]; then
    echo -e "$USAGE"
    exit 1
fi


# get directory from last argument (or assume current one)
if [[ ! "$1" =~ ^-(anum|mnum|f)$ ]]; then
    DIR=${1:-.}
else
    DIR=${2:-.}
fi


# populate $TC_FILES with files declared in CMakeLists.txt
get_tc_files $DIR
if [ $? != 0 ]; then
    exit 1
fi


# run appropriate subcommand
case "$1" in
    -anum)
        tc_anum2 $TC_FILES ;;
    -mnum)
        tc_mnum $TC_FILES ;;
    -f)
        tc_fullinfo $TC_FILES ;;
    *)
        tc_names $TC_FILES ;;
esac
