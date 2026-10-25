# SkillsOfTheWild.dll on alandtse's CommonLibSSE-NG; included by E:/WorkSpace/ng-build/CMakeLists.txt (see ng_plugin there)
ng_plugin(TARGET SkillsOfTheWild NAME SkillsOfTheWild VERSION 1.0.0 ROOT "${CMAKE_CURRENT_LIST_DIR}"
    SOURCES src/main.cpp src/Game.cpp src/Settings.cpp src/State.cpp src/Skills.cpp src/Synergy.cpp src/Hud.cpp src/Menu.cpp
    INCLUDES "${CMAKE_CURRENT_LIST_DIR}/src" "${CMAKE_CURRENT_LIST_DIR}/include"
    PCH "${CMAKE_CURRENT_LIST_DIR}/src/PCH.h")
