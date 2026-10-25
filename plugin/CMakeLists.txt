cmake_minimum_required(VERSION 3.21)

# SkillsOfTheWild.dll - the SKSE plugin behind Skills of the Wild 2026: the skill trees on the game's own events, the HUD rules, the settings pages
# in the SKSE Menu Framework and the links to Last Seed, Frostfall and Simple Hunting Overhaul. Built on CharmedBaryon's CommonLibSSE-NG (the "older" DLL,
# for Skyrim VR and 1.6.1130 and older); the newer DLL (1.6.1170 and 1.7.x) is built from the same sources by E:/WorkSpace/ng-build.
project(SkillsOfTheWild VERSION 1.0.1 LANGUAGES CXX)

find_package(CommonLibSSE CONFIG REQUIRED)

add_commonlibsse_plugin(${PROJECT_NAME} NAME "SkillsOfTheWild" AUTHOR "CageTV" SOURCES
    src/main.cpp
    src/Game.cpp
    src/Settings.cpp
    src/State.cpp
    src/Skills.cpp
    src/Synergy.cpp
    src/Hud.cpp
    src/Menu.cpp
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
target_include_directories(${PROJECT_NAME} PRIVATE src include)
target_precompile_headers(${PROJECT_NAME} PRIVATE src/PCH.h)
target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:/Z7>)
target_link_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:/DEBUG /OPT:REF /OPT:ICF>)
