#!/usr/bin/env bash

cd $(dirname $0)
PROJECT_PATH=$(pwd)

COMPILER_OPTIONS="-g2 -O3"

fetch_sources() {
    local SEARCH_PATH="$1"

    # '+ 1' because 'cut', which is used to cut off the path elements
    # before the 'src-c'-folder, starts counting characters from 1 onwards.
    ((LEN_FIND_PREFIX="${#SEARCH_PATH} + 2"))

    ((LAST_CHARACTER_OFFSET="${#SEARCH_PATH}-1"))
    local LAST_CHARACTER="${SEARCH_PATH:$LAST_CHARACTER_OFFSET:1}"

    # If there's a slash that is not needed, cut that away as well.
    if [[ $LAST_CHARACTER == '/' ]];
    then
        ((LEN_FIND_PREFIX--))
    fi

    local RAW_SOURCES=$(find "$SEARCH_PATH" -type f | grep ".c\$" | cut -c $LEN_FIND_PREFIX-)
    echo "$RAW_SOURCES"
}

make_object_name() {
    local SOURCE_UNIT=$1
    ((LEN_WITHOUT_EXTENSION=${#SOURCE_UNIT}-2))

    local RAW_NAME=$(echo $SOURCE_UNIT | tr "-" "_" | tr "/" "-" | cut -c -$LEN_WITHOUT_EXTENSION)
    echo $RAW_NAME.o
}



compile_source_unit() {
    local SOURCE_NAME=$1
    echo ">> $SOURCE_NAME"

    local OBJECT_NAME=$(make_object_name $SOURCE_NAME)
    gcc -c -fPIC $COMPILER_OPTIONS \
        -o "$PROJECT_PATH/.build/modules/core/objects/$OBJECT_NAME" \
        core/src-c/$SOURCE_NAME \
        -I $PROJECT_PATH/core/inc-c
}

build_core_library() {
    local SOURCE_LIST=$(fetch_sources "$PROJECT_PATH/core/src-c")

    mkdir -p $PROJECT_PATH/.build/modules/core/objects
    rm -f $PROJECT_PATH/.build/modules/core/objects/*.o

    for SOURCE_NAME in $SOURCE_LIST;
    do
        compile_source_unit "$SOURCE_NAME"
    done

    ar -rvs "$PROJECT_PATH/.build/libqueryxml.a" "$PROJECT_PATH/.build/modules/core/objects/"*.o

    gcc -shared \
        -o "$PROJECT_PATH/.build/libqueryxml.so" \
        $COMPILER_OPTIONS "$PROJECT_PATH/.build/modules/core/objects/"*.o
}



build_single_test() {
    local TEST_NAME="$1"
    local TEST_PATH="$PROJECT_PATH/core/tests/$TEST_NAME"
    local TEST_BUILD_PATH="$PROJECT_PATH/.build/modules/core/tests/$TEST_NAME"

    mkdir -p $TEST_BUILD_PATH/objects

    local SOURCE_LIST=$(fetch_sources $TEST_PATH/src-c)

    for SOURCE_NAME in $SOURCE_LIST;
    do
        local OBJECT_NAME=$(make_object_name $SOURCE_NAME)
        gcc -c $COMPILER_OPTIONS \
            -o $TEST_BUILD_PATH/objects/$OBJECT_NAME \
            $TEST_PATH/src-c/main.c \
            -I $PROJECT_PATH/core/inc-c \
            -I $TEST_PATH/inc-c
    done

    gcc \
        -o $TEST_BUILD_PATH/executable.elf \
        $TEST_BUILD_PATH/objects/*.o \
        $PROJECT_PATH/.build/libqueryxml.a
}

build_all_tests() {
    mkdir -p $PROJECT_PATH/.build/modules/core/tests

    for TEST_NAME in $(ls -A $PROJECT_PATH/core/tests);
    do
        local TEST_PATH="$PROJECT_PATH/core/tests/$TEST_NAME"

        if [[ ! -d $TEST_PATH ]];
        then
            continue
        fi

        build_single_test $TEST_NAME
    done
}



build_single_example() {
    local EXAMPLPE_NAME="$1"
    local EXAMPLE_PATH="$PROJECT_PATH/core/examples/$EXAMPLE_NAME"
    local EXAMPLE_BUILD_PATH="$PROJECT_PATH/.build/modules/core/examples/$EXAMPLE_NAME"

    mkdir -p $EXAMPLE_BUILD_PATH/objects

    local SOURCE_LIST=$(fetch_sources $EXAMPLE_PATH/src-c)

    for SOURCE_NAME in $SOURCE_LIST;
    do
        local OBJECT_NAME=$(make_object_name $SOURCE_NAME)
        gcc -c $COMPILER_OPTIONS \
            -o $EXAMPLE_BUILD_PATH/objects/$OBJECT_NAME \
            $EXAMPLE_PATH/src-c/main.c \
            -I $PROJECT_PATH/core/inc-c \
            -I $EXAMPLE_PATH/inc-c
    done

    gcc \
        -o $EXAMPLE_BUILD_PATH/executable.elf \
        $EXAMPLE_BUILD_PATH/objects/*.o \
        $PROJECT_PATH/.build/libqueryxml.a
}

build_all_examples() {
    mkdir -p $PROJECT_PATH/.build/modules/core/examples

    for EXAMPLE_NAME in $(ls -A $PROJECT_PATH/core/examples);
    do
        local EXAMPLE_PATH="$PROJECT_PATH/core/examples/$EXAMPLE_NAME"

        if [[ ! -d $EXAMPLE_PATH ]];
        then
            continue
        fi

        build_single_example $EXAMPLE_NAME
    done
}



list_all_examples() {
    EXAMPLE_LIST=$(ls -A $PROJECT_PATH/.build/modules/core/examples)

    for EXAMPLE in "$EXAMPLE_LIST";
    do
        if [[ ! -d "$PROJECT_PATH/.build/modules/core/examples" ]];
        then
            continue
        fi
        echo "- core:$EXAMPLE"
    done
}

list_all_tests() {
    TEST_LIST=$(ls -A $PROJECT_PATH/.build/modules/core/tests)

    for TEST in $TEST_LIST;
    do
        if [[ ! -d "$PROJECT_PATH/.build/modules/core/tests" ]];
        then
            continue
        fi
        echo "- core:$TEST"
    done
}



run_tests() {
    local TEST_NAME=$1

    local SPLITTER_INDEX=0
    while [[ $SPLITTER_INDEX -lt ${#TEST_NAME} ]];
    do
        local CHARACTER=${TEST_NAME:$SPLITTER_INDEX:1}
        if [[ $CHARACTER == ":" ]];
        then
            break
        fi
        ((SPLITTER_INDEX++))
    done

    local MODULE_NAME=$(echo $TEST_NAME | cut -c -$SPLITTER_INDEX)
    ((SPLITTER_INDEX += 2))
    local TEST_NAME=$(echo $TEST_NAME | cut -c $SPLITTER_INDEX-)

    local TEST_PATH="$PROJECT_PATH/.build/modules/$MODULE_NAME/tests/$TEST_NAME"
    if [[ ! -d "$TEST_PATH" ]];
    then
        echo "error: non-existent unit-test '$TEST_NAME'"
        return
    fi

    cd $TEST_PATH
    ./executable.elf
}

run_example() {
    local EXAMPLE_IDENTIFIER=$1

    local SPLITTER_INDEX=0
    while [[ $SPLITTER_INDEX -lt ${#EXAMPLE_IDENTIFIER} ]];
    do
        local CHARACTER=${EXAMPLE_IDENTIFIER:$SPLITTER_INDEX:1}
        if [[ $CHARACTER == ":" ]];
        then
            break
        fi
        ((SPLITTER_INDEX++))
    done

    local MODULE_NAME=$(echo $EXAMPLE_IDENTIFIER | cut -c -$SPLITTER_INDEX)
    ((SPLITTER_INDEX += 2))
    local EXAMPLE_NAME=$(echo $EXAMPLE_IDENTIFIER | cut -c $SPLITTER_INDEX-)

    local EXAMPLE_PATH="$PROJECT_PATH/.build/modules/$MODULE_NAME/examples/$EXAMPLE_NAME"
    if [[ ! -d "$EXAMPLE_PATH" ]];
    then
        echo "error: non-existent example '$EXAMPLE_NAME'"
        return
    fi

    cd $EXAMPLE_PATH
    ./executable.elf
}


case $1 in
    "b" | "build-all")
        build_core_library "${@:2}"
        build_all_examples "${@:2}"
        build_all_tests "${@:2}"
        ;;

    "bl" | "build-library")
        build_core_library "${@:2}"
        ;;

    "bx" | "build-examples")
        build_all_examples "${@:2}"
        ;;

    "bt" | "build-tests")
        build_all_tests "${@:2}"
        ;;

    "lx" | "list-examples")
        list_all_examples "${@:2}"
        ;;

    "lt" | "list-tests")
        list_all_tests "${@:2}"
        ;;

    "rx" | "run-example")
        run_example "${@:2}"
        ;;

    "rt" | "run-tests")
        run_tests "${@:2}"
        ;;

    "h" | "help")
        cat $PROJECT_PATH/resources/help-texts/main.txt
        ;;

    *)
        echo "error: unknown action '$1'"
        echo "try: $0 help"
        ;;
esac
