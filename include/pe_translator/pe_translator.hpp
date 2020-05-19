#ifndef PE_TRANSLATOR_HPP
#define PE_TRANSLATOR_HPP

#include "pe_support.hpp"
#include "utils/strings.hpp"
#include "utils/vector.hpp"

namespace shush::pe_trans {

namespace detail {

const char INSTRUCTIONS_FILE[] = PATH_PREFIX "submodules/shushlang/inc/create_commands.py";
const size_t MAX_INSTRUCTION_DESCRIPTION_LEN = 2048; // max chars in a line in create_commands

const char   TEMPLATE_PE32_FILE_PATH[] = PATH_PREFIX "resources/nops_adv.exe";
const size_t TEMPLATE_PE32_CODE_START  = 0x400;

const char   TEMPLATE_ASM_FILE_PATH[]  = PATH_PREFIX "resources/template.asm";
const size_t TEMPLATE_ASM_CODE_START   =   480;

const uint32_t DEFAULT_BASE_RVA = 0x401010; // start: address.

enum class TranslationWritingMode {
  APPEND,
  WRITE
};

/**
 * Structure for filling RVAs in call and jump commands.
 */
struct AddressInfo {
  size_t in_file_com_byte;
  size_t out_file_rva_byte;
};

inline bool operator<(const AddressInfo& info_1, const AddressInfo& info_2) {
  return info_1.in_file_com_byte < info_2.in_file_com_byte;
}

}

namespace inst {
static const uint8_t MAX_INSTRUCTION_SIZE = 255; // For both architectures.

/**
 * Structure which is created from a shushexe instruction, which is ready to be converted
 * to the other format.
 */
struct ShushInstructionConverted {
  uint8_t size_in  = 0;
  uint8_t size_out = 0;

  char out_res[MAX_INSTRUCTION_SIZE] {};
};

/**
 * Describes possible variations of arguments of a shush instruction
 */
struct ShushArgs {
  enum class Type {
    DOUBLE,
    REGISTER,
    ADDRESS,
    NO_TYPE
  } type;
  size_t size;
  union {
    double as_double;
    uint8_t as_reg;
    uint64_t as_address;
  } value = {};
};

/**
 * A structure that tells us about the structure of a shush instruction and how to convert it
 * to other format.
 */
struct ShushInstructionInfo {
  uint8_t shush_opcode = 0;
  ShushArgs shush_args = {};
  char shush_name[MAX_INSTRUCTION_SIZE] {};
};

}

enum class TranslationMode {
  TRANSLATE_TO_PE32,
  TRANSLATE_TO_ASM_CODE
};

enum class TranslationError {
  NO_ERROR = 0,
  ERROR_INVALID_COMMAND
};

class Translator {
public:
  Translator(FILE* in, FILE* out, const TranslationMode mode);

  TranslationError Translate();

private:
  static detail::TranslationWritingMode GetTranslationWritingMode(const TranslationMode mode);

  /**
   * Loads shush instruction info from submodule.
   */
  void LoadInstructions();

  /**
   * Translates most of commands, excluding calls and jumps.
   * Finds positions of commands in shushexe that are to be
   * converted to RVA later.
   */
  TranslationError FirstPass ();
  /**
   * Finds all RVAs needed for jumps and calls. Fills them.
   */
  TranslationError SecondPass();


  /**
   * Converts current pos in the out file to RVA and fills it
   * at where.
   */
  void FillRva(const size_t where) const;

  /**
   * Is the file indicator in `in` file reached its end? 
   */
  bool IsEndOfShushFile() const;

  /**
   * Works once both files are ready; increases offsets on both in and out files
   * so that the instruction they will point to are respected. 
   */
  void IncrementFileOffsets();


  /**
   * By given shush instruction info, generates an instruction in needed format.
   */
  inst::ShushInstructionConverted ConvertInstruction(inst::ShushInstructionInfo* info);
  /**
   * Parses current instruction pointed to py in file's indicator. Returns
   * ShushInstruction which holds all needed data, even the resulting opcodes.
   */
  inst::ShushInstructionConverted ParseCurrentShushInstruction();

  /**
   * Searches inst_info to find a matching opcode.
   */
  inst::ShushInstructionInfo* FindShushInstructionInfoByOpcode(char opcode);


  Vector<inst::ShushInstructionInfo> inst_info;
  Vector<detail::AddressInfo> addrs_to_fill;

  FILE* in;
  FILE* out;
  const TranslationMode mode;
  const detail::TranslationWritingMode write_mode;
};

}

#endif // PE_TRANSLATOR_HPP
