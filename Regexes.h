#ifndef _REGEXES_H
#define _REGEXES_H

#include <regex>
#include <string>

using namespace std;

regex direktives(".+");
regex comments("([^#]*)#.*");
regex filter_comment("(#)(.*)");
regex tab("\\t");
regex label("[a-zA-Z][a-zA-Z0-9_]*\\:");

regex psw("(psw)");
regex sp("(sp)");
regex pc("(pc)");

regex decimal_num("-?[0-9]+");
regex hexa_num("^(0(x|X)([0-9]|[A-Fa-f])+)$");
regex symbol("[a-zA-Z][a-zA-Z0-9_]*");
regex reg_num("[0-7]");

regex filter_from_direktives("[a-zA-Z][a-zA-Z0-9_]*( )*");
regex filter_from_word("[a-zA-Z0-9_]+ ?,?");
regex filter_from_instruction("(r|R)[0-9]+ ?,?");
regex filter_from_add("[a-zA-Z]");

regex global_directive(".(global)(.*)");
regex global_directive_replace(".(global)");
regex extern_directive(".(extern)(.*)");
regex extern_directive_replace(".(extern)");
regex section_directive(".(section)(.*)");
regex section_directive_replace(".(section)");
regex word_directive(".(word)(.*)");
regex word_directive_replace(".(word)");
regex skip_directive(".(skip)(.*)");
regex skip_directive_replace(".(skip)");
regex end_directive(".(end)");
regex ascii_directive(".(ascii)(.*)");
regex ascii_directive_replace(".(ascii)");
regex equ_directive(".(equ)(.*)");
regex equ_directive_filter(".(equ)");

regex halt_instr("(halt)");
regex add_instr("(add)(.*)");
regex add_instr_filter("(add)");
regex sub_instr("(sub)(.*)");
regex sub_instr_filter("(sub)");
regex mul_instr("(mul)(.*)");
regex mul_instr_filter("(mul)");
regex div_instr("(div)(.*)");
regex div_instr_filter("(div)");
regex cmp_instr("(cmp)(.*)");
regex cmp_instr_filter("(cmp)");
regex and_instr("(and)(.*)");
regex and_instr_filter("(and)");
regex or_instr("(or)(.*)");
regex or_instr_filter("(or)");
regex xor_instr("(xor)(.*)");
regex xor_instr_filter("(xor)");
regex test_instr("(test)(.*)");
regex test_instr_filter("(test)");
regex shl_instr("(shl)(.*)");
regex shl_instr_filter("(shl)");
regex shr_instr("(shr)(.*)");
regex shr_instr_filter("(shr)");
regex not_instr("(not)(.*)");
regex not_instr_filter("(not)");
regex ret_instr("(ret)");
regex iret_instr("(iret)");
regex int_instr("(int)(.*)");
regex int_instr_filter("(int)");
regex xchg_instr("(xchg)(.*)");
regex xchg_instr_filter("(xchg)");
regex call_instr("(call)(.*)");
regex call_instr_filter("(call)");
regex ldr_instr("(ldr)(.*)");
regex ldr_instr_filter("(ldr)");
regex str_instr("(str)(.*)");
regex str_instr_filter("(str)");

regex zvezdica("\\*(.*)");
regex zvezdica_filter("\\*");
regex dolar("\\$(.*)");
regex dolar_filter("\\$");
regex clasic_literal("(0(x|X))*[0-9]+");
regex clasic_symbol("[a-zA-Z][a-zA-Z0-9_]+");
regex percent("\\%(.*)");
regex percent_filter("\\%");
regex register_adressing("(r|R)[0-9]+");
regex mem_register_adressing("\\[(r|R)[0-9]+\\]");
regex first_indirekt_filter("\\[");
regex second_indirekt_filter("\\]");
regex plus_filter("\\+");
regex mem_register_adressing_mov_literal("\\[(r|R)[0-9]+\\+[0-9]+\\]+");
regex mem_register_adressing_mov_symbol("\\[(r|R)[0-9]+\\+[a-zA-Z0-9_]+\\]");

regex jmp_instr("(jmp)(.*)");
regex jmp_instr_filter("(jmp)");
regex jeq_instr("(jeq)(.*)");
regex jeq_instr_filter("(jeq)");
regex jne_instr("(jne)(.*)");
regex jne_instr_filter("(jne)");
regex jgt_instr("(jgt)(.*)");
regex jgt_instr_filter("(jgt)");
regex push_pop_instr("(push|pop)(.*)");
regex push_pop_filter("(push|pop)");

// regex label_only("^(" + str_symbol + "):$");             // nothing is after label
// regex label_with_command("^(" + str_symbol + "):(.*)$"); // something is after label
// regex symbol("^(" + str_symbol + ")$");
// regex decimal_num("^(" + str_decimal_num + ")$");
// regex hexadecimal_num("^(" + str_hexa_num + ")$");

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
// regex rx_jmp_address_syntax_notation_absolute("^(" + symbol_or_literal + ")$");
// regex rx_jmp_address_syntax_notation_memdir("^\\*(" + symbol_or_literal + ")$");
// regex rx_jmp_address_syntax_notation_symbol_pc_relative("^%(" + str_symbol + ")$");
// regex rx_jmp_address_syntax_notation_regdir("^\\*(r[0-7]|psw)$");
// regex rx_jmp_address_syntax_notation_regind("^\\*\\[(r[0-7]|psw)\\]$");
// regex rx_jmp_address_syntax_notation_regind_with_displacement("^\\*\\[(r[0-7]|psw) \\+ (" + symbol_or_literal + ")\\]$");

// // syntax notation for operand in instruction for load/store
// regex rx_load_store_address_syntax_notation_absolute("^\\$(" + symbol_or_literal + ")$");
// regex rx_load_store_address_syntax_notation_memdir("^(" + symbol_or_literal + ")$");
// regex rx_load_store_address_syntax_notation_pc_relative("^%(" + str_symbol + ")$");
// regex rx_load_store_address_syntax_notation_regdir("^(r[0-7]|psw)$");
// regex rx_load_store_address_syntax_notation_regind("^\\[(r[0-7]|psw)\\]$");
// regex rx_load_store_address_syntax_notation_regind_with_displacement("^\\[(r[0-7]|psw) \\+ (" + symbol_or_literal + ")\\]$");

// syntax notation for operand in instruction for load or store

#endif // ASSEMBLY_REGEXES_H