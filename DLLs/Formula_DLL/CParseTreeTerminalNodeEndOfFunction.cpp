//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose:
//
//******************************************************************************

 #include "CParseTreeTerminalNodeEndOfFunction.h"
///#include "CMemoryPool.h"
#include "COHScriptObject.h"
#include "CTokenizer.h"
#include "..\Debug_DLL\debug.h"
#include "..\Globals_DLL\globals.h"
#include "..\Preferences_DLL\Preferences.h"
#include "..\Symbols_DLL\CEngineContainer.h"
#include "..\Symbols_DLL\CFunctionCollection.h"

// static
bool CParseTreeTerminalNodeEndOfFunction::_evaluating_defailt_logic = false;

CParseTreeTerminalNodeEndOfFunction::CParseTreeTerminalNodeEndOfFunction(int relative_line_number) : 
  // Increment line number by 1 
  // to point to the first empty line after the function code
  // to avoid confusion and emphasize that we reached the end.
  CParseTreeTerminalNodeIdentifier((relative_line_number + 1), "") {
  CString currenttly_parsed_function = CTokenizer::CurrentFunctionName();
  write_log(Preferences()->debug_parser(), "[CParseTreeTerminalNodeEndOfFunction] %s\n",
    currenttly_parsed_function);
  if (COHScriptObject::IsMainOpenPPLFunction(currenttly_parsed_function)) {
    assert(currenttly_parsed_function.Left(2) == "f$");
    // Build new Name: "Default" + old name without "f$" 
    // plus uppercase for the first character of the 2nd part,
    // e.g. f$preflop -> DefaultPreflop
    CString default_function = "Default" + currenttly_parsed_function.Mid(2);
    default_function.SetAt(7, toupper(default_function.GetAt(7)));
    write_log(Preferences()->debug_parser(), "[CParseTreeTerminalNodeEndOfFunction] %s\n",
      default_function);
    if (FunctionCollection()->Exists(default_function)) {
      _terminal_name = default_function;
      write_log(Preferences()->debug_parser(), 
        "[CParseTreeTerminalNodeEndOfFunction] Default bot-logic referenced.\n");
      return;
    }
  }
  // Not a main OPPL-function or fallback-logic does not exist
  write_log(Preferences()->debug_parser(),
    "[CParseTreeTerminalNodeEndOfFunction] Falling back to %s\n",
    kEmptyExpression_False_Zero_WhenOthersFoldForce);
  _terminal_name = kEmptyExpression_False_Zero_WhenOthersFoldForce;
}

CParseTreeTerminalNodeEndOfFunction::~CParseTreeTerminalNodeEndOfFunction() {
}

double CParseTreeTerminalNodeEndOfFunction::Evaluate(bool log /* = false */) {
  if (memcmp(_terminal_name, "Default", 7) == 0) {
    _evaluating_defailt_logic = true;
    return CParseTreeTerminalNodeIdentifier::Evaluate(log);
    _evaluating_defailt_logic = false;
  } else {
    return CParseTreeTerminalNodeIdentifier::Evaluate(log);
  }

}