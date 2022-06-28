#ifndef _REGEXES_H
#define _REGEXES_H
#include <regex>
#include <string>
using namespace std;

regex direktives("(.)+");
regex comments("([^#]*)#.*");
regex tab("\\t");
// regex new_line("\\n");    nije potrebno?

string str_decimal_num = "-?[0-9]+";         // this is a decimal number (positive or negative)
string str_hexa_num = "0x[0-9A-F]+";         // this is a hexadecimal number
string str_symbol = "[a-zA-Z][a-zA-Z0-9_]*"; // symbol can start only with letter and can contain letters, digits and _
string str_reg_num = "[0-7]";                // there are 8 registers: r0 - r7

string symbol_or_literal = str_symbol + "|" + str_decimal_num + "|" + str_hexa_num;

regex global_directive("^\\.global (" + str_symbol + "(," + str_symbol + ")*)$");
regex extern_directive(".(extern)(.*)");
regex section_directive("^\\.section (" + str_symbol + ")$");
regex word_directive("^\\.word ((" + symbol_or_literal + ")(,(" + symbol_or_literal + "))*)$");
regex skip_directive("^\\.skip (" + str_decimal_num + "|" + str_hexa_num + ")$");
regex end_directive("^\\.end$");

regex label_only("^(" + str_symbol + "):$");             // nothing is after label
regex label_with_command("^(" + str_symbol + "):(.*)$"); // something is after label
regex symbol("^(" + str_symbol + ")$");
regex decimal_num("^(" + str_decimal_num + ")$");
regex hexadecimal_num("^(" + str_hexa_num + ")$");

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// regex rx_find_extra_spaces(" {2,}");
// // regex rx_find_empty_strings(""); // this lines will be skipped manually
// regex rx_remove_boundary_spaces("^( *)([^ ].*[^ ])( *)$");
// regex rx_find_comma_spaces(" ?, ?");
// regex rx_find_columns_spaces(" ?: ?");

// Odavde treba proveriti nadole:

// labels are in form -> 'symbol:'
// label has to be at the beginning of the line
// if there is not anything, label is associated with next command

// instructions:
// instructions that operate only with registers
regex rx_no_operand_instruction("^(halt|iret|ret)$");
regex rx_one_operand_register_instruction("^(push|pop|int|not) (r[0-7]|psw)$");
regex rx_two_operand_register_instruction("^(xchg|add|sub|mul|div|cmp|and|or|xor|test|shl|shr) (r[0-7]|psw),(r[0-7]|psw)$");

// instructions with operand (all kind)
// jumps
regex rx_one_operand_all_kind_addressing_jumps("^(call|jmp|jeq|jne|jgt) (.*)$");
// load/store
regex rx_two_operand_all_kind_addressing_load_store("^(ldr|str) (r[0-7]|psw),(.*)$");

// syntax notation for operand in instruction for jumps
regex rx_jmp_address_syntax_notation_absolute("^(" + symbol_or_literal + ")$");
regex rx_jmp_address_syntax_notation_memdir("^\\*(" + symbol_or_literal + ")$");
regex rx_jmp_address_syntax_notation_symbol_pc_relative("^%(" + str_symbol + ")$");
regex rx_jmp_address_syntax_notation_regdir("^\\*(r[0-7]|psw)$");
regex rx_jmp_address_syntax_notation_regind("^\\*\\[(r[0-7]|psw)\\]$");
regex rx_jmp_address_syntax_notation_regind_with_displacement("^\\*\\[(r[0-7]|psw) \\+ (" + symbol_or_literal + ")\\]$");

// syntax notation for operand in instruction for load/store
regex rx_load_store_address_syntax_notation_absolute("^\\$(" + symbol_or_literal + ")$");
regex rx_load_store_address_syntax_notation_memdir("^(" + symbol_or_literal + ")$");
regex rx_load_store_address_syntax_notation_pc_relative("^%(" + str_symbol + ")$");
regex rx_load_store_address_syntax_notation_regdir("^(r[0-7]|psw)$");
regex rx_load_store_address_syntax_notation_regind("^\\[(r[0-7]|psw)\\]$");
regex rx_load_store_address_syntax_notation_regind_with_displacement("^\\[(r[0-7]|psw) \\+ (" + symbol_or_literal + ")\\]$");

// syntax notation for operand in instruction for load or store

#endif // ASSEMBLY_REGEXES_H