
MACRO(EVAL_CONDITION name)
   IF(${ARGN})
     SET(${name} 1)
   ELSE(${ARGN})
     SET(${name} 0)
   ENDIF(${ARGN})
ENDMACRO(EVAL_CONDITION)

