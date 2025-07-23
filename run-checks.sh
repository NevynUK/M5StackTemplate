#!/bin/bash

set -e
scriptdir="$( cd "$(dirname "$0")" ; pwd -P )"

VERBOSE_FLAGS=
HELP=false

#
#   Work out which options are on the command line.
#
for i in "$@"
do
case $i in
    -v|--verbose)
    VERBOSE_FLAGS=--verbose
    ;;
    -h|--help)
    HELP=true
    ;;
    *)
    printf "Uknown command line option ($i)."
    printf "Run run-checks.sh --help for information on valid options."
    exit 1
    ;;
esac
done

if $HELP; then
    echo "$0: Run code checks."
    echo ""
    echo "Options:"
    echo "-h | --help             Display this message"
    echo "-v | --verbose          Execute in verbose mode (useful for debugging)"
    echo ""
    exit 0
fi


SOURCE_DIR=$scriptdir
COMPONENTS_DIR=components

echo "Running cppcheck"
cppcheck --error-exitcode=1 --quiet --check-level=exhaustive --force --inline-suppr -iCMakeLists.txt -ibuild  -imanaged_components $SOURCE_DIR
#
#   The following uses the compile_commands.json file - needs uplift to latest SDK.
#
# cppcheck --check-level=exhaustive --force --inline-suppr -iCMakeLists.txt -iSource/MeadowComms/components/esp_littlefs -iSource/MeadowComms/build --project=$scriptdir/Source/MeadowComms/build/compile_commands.json

echo "Running clang-format"
find main -name "*.?pp" -exec clang-format --Werror -dry-run {} \;
find components -name "*.?pp" -exec clang-format --Werror -dry-run {} \;