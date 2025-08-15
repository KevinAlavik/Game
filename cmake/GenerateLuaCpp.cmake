file(READ "${INPUT}" LUA_CODE)

set(RAW_DELIM "GAME_LUA")
file(WRITE "${OUTPUT}" "#pragma once\nconstexpr const char* GameLuaCode = R\"${RAW_DELIM}(\n${LUA_CODE}\n)${RAW_DELIM}\";\n")
