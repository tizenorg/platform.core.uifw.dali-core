#!/bin/bash

TEMP=`getopt -o sr --long serial,rerun -n 'execute.sh' -- "$@"`

if [ $? != 0 ] ; then echo "Terminating..." >&2 ; exit 1 ; fi

# Note the quotes around `$TEMP': they are essential!
eval set -- "$TEMP"

opt_serial=0
opt_rerun=""
while true ; do
    case "$1" in
        -s|--serial)   opt_serial=1 ; shift ;;
        -r|--rerun)    opt_rerun="-r" ; shift ;;
        --) shift; break;;
	*) echo "Internal error $1!" ; exit 1 ;;
    esac
done

function execute
{
    scripts/tctestsgen.sh $1 `pwd` desktop $2
    testkit-lite -f `pwd`/tests.xml -o tct-${1}-core-tests.xml  -A --comm localhost
    scripts/add_style.pl $1
}

# Clean up old test results
rm -f tct*core-tests.xml

# Clean up old coverage data
if [ -d ../build/tizen ] ; then
    rm -f ../build/tizen/dali-core/.libs/*.gcda
fi

find build \( -name "*.gcda" \) -exec rm '{}' \;

if [ $opt_serial = 1 ] ; then
    # Run all test case executables serially, create XML output
    if [ -n "$1" ] ; then
        execute $1 $*
    else
        for mod in `ls -1 src/ | grep -v CMakeList `
        do
            if [ $mod != 'common' ] && [ $mod != 'manual' ]; then
                echo EXECUTING $mod
                execute $mod $*
            fi
        done
    fi

    scripts/summarize.pl
else
    # if $1 is an executable filename, execute it·

    if [ -z "$1" ] ; then
        # No arguments:
        # Execute each test executable in turn, using parallel execution
        for mod in `ls -1 src/ | grep -v CMakeList | grep -v common | grep -v manual`
        do
            echo -e "\nEXECUTING $mod"
            build/src/$mod/tct-$mod-core $opt_rerun
        done

    elif [ -f "build/src/$1/tct-$1-core" ] ; then
        # First argument is an executable filename - execute only that with any
        # remaining arguments
        module=$1
        shift;
        build/src/$module/tct-$module-core $opt_rerun $*

    else
       # First argument is not an executable. Is it a test case name?
       # Try executing each executable with the test case name until success/known failure
        for mod in `ls -1 src/ | grep -v CMakeList | grep -v common | grep -v manual`
        do
            output=`build/src/$mod/tct-$mod-core $1`
            ret=$?
            if [ $ret -ne 6 ] ; then
               echo $output
               if [ $ret -eq 0 ] ; then echo -e "\nPassed" ; fi
               exit $ret
            fi
        done
        echo $1 not found
    fi
fi
