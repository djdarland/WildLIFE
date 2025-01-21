# CMakeList.txt : CMake project for CPM_WL01, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("CPM_WL01")

# Add source to this project's executable.
add_executable (CPM_WL01 "CPM_WL01.cpp" "CPM_WL01.h" "Pract.h" "built_ins.h" "copy.h" "c_life.h" "defs.h" "defs_static.h" "def_config.h" "def_const.h" "def_glob.h" "def_macro.h" "def_proto.h" "def_struct.h" "error.h" "extern.h" "info.h" "interrupt.h" "lefun.h" "list.h" "login.h" "memory.h" "modules.h" "parser.h" "print.h" "raw.h" "sys.h" "templates.h" "token.h" "trees.h" "types.h" "arity.cpp" "bi_math.cpp" "bi_sys.cpp" "bi_type.cpp" "built_ins.cpp" "copy.cpp" "error.cpp" "hash_table.cpp" "info.cpp" "interrupt.cpp" "lefun.cpp" "lib.cpp" "life.cpp" "list.cpp" "login.cpp" "lub.cpp" "memory.cpp" "modules.cpp" "parser.cpp" "print.cpp" "raw.cpp" "sys.cpp" "sys_dbm.cpp" "templates.cpp" "token.cpp" "trees.cpp" "types.cpp" "regerror.cpp" "regexp.cpp" "regexp.h" "regmagic.h" "regsub.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET CPM_WL01 PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
