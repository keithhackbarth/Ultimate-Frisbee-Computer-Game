/*
See "CarnegieMellonGraphics.h" for library version info and copyright information
$Id: NamedKey.cpp,v 1.1.1.1 2002/10/19 19:02:31 jsaks Exp $
*/

#include <CarnegieMellonGraphics.h>

// Define NamedKey Constants

// Base value;
const int NamedKey::BASE = 127;

// Non-displayable ASCII characters
const int NamedKey::BACKSPACE   = 8;
const int NamedKey::ENTER       = 13;
const int NamedKey::ESCAPE      = 27;
const int NamedKey::DELETE_KEY  = 127;

// Function keys
const int NamedKey::FUNCTION_1  = NamedKey::BASE + 1;
const int NamedKey::FUNCTION_2  = NamedKey::BASE + 2;
const int NamedKey::FUNCTION_3  = NamedKey::BASE + 3;
const int NamedKey::FUNCTION_4  = NamedKey::BASE + 4;
const int NamedKey::FUNCTION_5  = NamedKey::BASE + 5;
const int NamedKey::FUNCTION_6  = NamedKey::BASE + 6;
const int NamedKey::FUNCTION_7  = NamedKey::BASE + 7;
const int NamedKey::FUNCTION_8  = NamedKey::BASE + 8;
const int NamedKey::FUNCTION_9  = NamedKey::BASE + 9;
const int NamedKey::FUNCTION_10 = NamedKey::BASE + 10;
const int NamedKey::FUNCTION_11 = NamedKey::BASE + 11;
const int NamedKey::FUNCTION_12 = NamedKey::BASE + 12;
 
// Arrow keys
const int NamedKey::LEFT_ARROW  = NamedKey::BASE + 13;
const int NamedKey::UP_ARROW    = NamedKey::BASE + 14;
const int NamedKey::RIGHT_ARROW = NamedKey::BASE + 15;
const int NamedKey::DOWN_ARROW  = NamedKey::BASE + 16;

  // Other keys
const int NamedKey::PAGE_UP     = NamedKey::BASE + 17;
const int NamedKey::PAGE_DOWN   = NamedKey::BASE + 18;
const int NamedKey::HOME        = NamedKey::BASE + 19;
const int NamedKey::END         = NamedKey::BASE + 20;
const int NamedKey::INSERT      = NamedKey::BASE + 21;

const int NamedKey::LAST = NamedKey::INSERT;
