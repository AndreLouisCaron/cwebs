# Copyright (c) 2011-2012, Andre Caron (andre.l.caron@gmail.com)
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
# 
#   Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
# 
#   Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# webs library unit tests.

macro(add_test_program name)
  add_executable(${name} ${name}.cpp)
  target_link_libraries(${name} webs sha1)
  add_dependencies(${name} webs sha1)
endmacro()

# compile the test program(s).
add_test_program(invalid-extension)
add_test_program(unknown-message-type)
add_test_program(message-type-change)
add_test_program(summarize-messages)

# self-contained tests.
add_test(invalid-extension invalid-extension)
add_test(unknown-message-type unknown-message-type)
add_test(message-type-change message-type-change)

# shortcut for invoking 'summarize-messages' and checking outputs.
macro(check_summary name input)
  add_test(${name}
    "${CMAKE_CURRENT_BINARY_DIR}/summarize-messages"
    "${CMAKE_CURRENT_SOURCE_DIR}/data/${input}"
  )
  foreach(result ${ARGN})
    set_tests_properties(${name}
      PROPERTIES
      PASS_REGULAR_EXPRESSION ${result}
    )
  endforeach()
endmacro()

# register tests.
check_summary(001 "simple-ping"
  "ping,p,5,f7ff9e8b7bb2e09b70935a5d785e0cc5d9d0abf0"
)
check_summary(002 "simple-pong"
  "pong,m,5,f7ff9e8b7bb2e09b70935a5d785e0cc5d9d0abf0"
)
check_summary(003 "simple-unmasked-text-message"
  "text,p,5,f7ff9e8b7bb2e09b70935a5d785e0cc5d9d0abf0"
)
check_summary(004 "simple-masked-text-message"
  "text,m,5,f7ff9e8b7bb2e09b70935a5d785e0cc5d9d0abf0"
)
check_summary(005 "fragmented-unmasked-text-message"
  "text,p,3,dbc2d1fed0dc37a70aea0f376958c802eddc0559"
  "text,p,2,638e8f0171575864326f06d2a5f8e72287427b15"
)
check_summary(006 "medium-binary-message"
  "data,p,256,22edfe35c32456210731c388df8aa3e5f8cff227"
)
