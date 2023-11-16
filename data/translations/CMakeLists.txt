set(locales
   de
   pl
)

list(TRANSFORM locales
   PREPEND ${CMAKE_CURRENT_LIST_DIR}/translation_
   OUTPUT_VARIABLE translation_files
)
list(TRANSFORM translation_files
   APPEND .ts
)

# expose list as ${TS_FILES} to parent scope
set(TS_FILES ${translation_files} PARENT_SCOPE)

# expose comma-separated list of locales to parent scope
string(REPLACE ";" "," translations "${locales}")
set(TRANSLATIONS ${translations} PARENT_SCOPE)
