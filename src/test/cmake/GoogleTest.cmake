# CMake script for GoogleTest
#
# requires:
#   ${CMAKE_GENERATOR}
#   ${GENERATOR_ARGS}

# Define the GoogleTest's path
set(GTEST_BUILD_DIR "${CMAKE_BINARY_DIR}/GoogleTest")
# インストール先は lib に統一（-D のジェネレータ式は展開されず失敗するため）。--install の --config で Debug/Release を切り替える。
set(GTEST_PC "${CMAKE_BINARY_DIR}/lib/pkgconfig/gtest.pc")

# GoogleTest サブプロジェクトの CMakeCache に CMAKE_INSTALL_LIBDIR=lib$<$<CONFIG:Debug>:/Debug> のような
# 誤った文字列が入ると cmake --install が失敗する。初期キャッシュで lib に固定する。
set(GTEST_INIT_CACHE "${CMAKE_BINARY_DIR}/gtest-init.cmake")
file(WRITE "${GTEST_INIT_CACHE}" "set(CMAKE_INSTALL_LIBDIR \"lib\" CACHE PATH \"Object code libraries (gtest)\" FORCE)\n")

message(STATUS "GoogleTest config: cmake -G \"${CMAKE_GENERATOR}\" ${GENERATOR_ARGS} -S \"${CMAKE_SOURCE_DIR}/externals/googletest\" -B \"${GTEST_BUILD_DIR}\" \"-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}\" ${GENERATOR_ARGS_FOR_STATIC_LIBRARY}")

add_custom_command(
  OUTPUT
    "${CMAKE_SOURCE_DIR}/externals/googletest/.git"
  COMMAND ${CMAKE_COMMAND}
    -DGIT_EXECUTABLE:FILEPATH=${GIT_EXECUTABLE}
    -DREPO_ROOT:PATH=${CMAKE_SOURCE_DIR}
    -DSUBMODULE_PATH:STRING=externals/googletest
    -DLOCK_PATH:FILEPATH=${CMAKE_BINARY_DIR}/cmake-submodule-update.lock
    -P ${CMAKE_SOURCE_DIR}/src/main/cmake/git_submodule_update_locked.cmake
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  COMMENT "Fetching GoogleTest's source files"
)

add_custom_target(fetch_gtest_source_files
  DEPENDS
    "${CMAKE_SOURCE_DIR}/externals/googletest/.git"
)

# サクラ本体と同じ静的ランタイム（/MT と /MTd）を明示。GENERATOR_ARGS だけだと展開失敗時に MultiThreaded 固定になり LNK2038 になる。
if(MSVC)
  set(GTEST_MSVC_RUNTIME_LIB "-DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

add_custom_command(
  OUTPUT "${GTEST_BUILD_DIR}/CMakeCache.txt"
  COMMAND ${CMAKE_COMMAND}
    -C "${GTEST_INIT_CACHE}"
    -G "${CMAKE_GENERATOR}"
    ${GENERATOR_ARGS}
    -S "${CMAKE_SOURCE_DIR}/externals/googletest"
    -B "${GTEST_BUILD_DIR}"
    "-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}"
    "-DCMAKE_INSTALL_LIBDIR=lib"
    ${GTEST_MSVC_RUNTIME_LIB}
    ${GENERATOR_ARGS_FOR_STATIC_LIBRARY}
    -DBUILD_GMOCK=ON
    -Dgtest_build_tests=OFF
    -Dgtest_build_samples=OFF
  DEPENDS fetch_gtest_source_files
  COMMENT "Configure GoogleTest Library"
)

add_custom_target(configure_gtest
  DEPENDS
    "${GTEST_BUILD_DIR}/CMakeCache.txt"
)

add_custom_command(
  OUTPUT "${GTEST_PC}"
  COMMAND ${CMAKE_COMMAND} --build "${GTEST_BUILD_DIR}" --config $<CONFIG>
  COMMAND ${CMAKE_COMMAND} --install "${GTEST_BUILD_DIR}" --config $<CONFIG>
  # install は常に CMAKE_INSTALL_PREFIX/lib に上書きするため、MSBuild が Debug/Release を切替えても
  # 直前にビルドした構成の .lib が残る。tests1 は $(Configuration) 別フォルダを参照する。
  COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/lib/$<CONFIG>"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_BINARY_DIR}/lib/gtest.lib" "${CMAKE_BINARY_DIR}/lib/$<CONFIG>/gtest.lib"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_BINARY_DIR}/lib/gmock.lib" "${CMAKE_BINARY_DIR}/lib/$<CONFIG>/gmock.lib"
  DEPENDS configure_gtest
  COMMENT "Building GoogleTest Library"
)

add_custom_target(generate_gtest
  DEPENDS
    "${GTEST_PC}"
)
