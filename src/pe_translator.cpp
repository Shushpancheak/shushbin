#include "pe_translator/pe_translator.hpp"

#include "pe_translator/instructions.hpp"

shush::pe_trans::Translator::Translator(FILE* in, FILE* out, const TranslationMode mode)
  : in(in)
  , out(out)
  , mode(mode)
  , write_mode(GetTranslationWritingMode(mode)) {
  LoadInstructions();
}

shush::pe_trans::TranslationError shush::pe_trans::Translator::Translate() {
  FILE* template_file = nullptr;
  fopen_s(&template_file, detail::TEMPLATE_PE32_FILE_PATH, "rb");

  const size_t copy_buf_size = 1024;
  char buf[copy_buf_size] {};

  while (!feof(template_file)) {
    fread(buf, sizeof(char), copy_buf_size, template_file);
    fwrite(buf, sizeof(char), copy_buf_size, out);
  }

  fclose(template_file);
  fseek(out, 0, SEEK_SET);

  auto res = FirstPass();
  if (res != TranslationError::NO_ERROR) {
    return res;
  }

  res = SecondPass();
  return res;
}

shush::pe_trans::detail::TranslationWritingMode shush::pe_trans::Translator::GetTranslationWritingMode(
  const TranslationMode mode) {
  if (mode == TranslationMode::TRANSLATE_TO_PE32) {
    return detail::TranslationWritingMode::WRITE;
  } else {
    return detail::TranslationWritingMode::APPEND;
  }
}

void shush::pe_trans::Translator::LoadInstructions() {
  FILE* inst_file = nullptr;
  fopen_s(&inst_file, detail::INSTRUCTIONS_FILE, "r");

  char buf[detail::MAX_INSTRUCTION_DESCRIPTION_LEN] {};

  while (!feof(inst_file)) {
    fgets(buf, detail::MAX_INSTRUCTION_DESCRIPTION_LEN, inst_file);

    inst::ShushInstructionInfo new_inst = {};

    size_t offset = 0;
    size_t name_off = 0;
    size_t i = 0;
    for (; buf[i] != '\0'; ++i) {
      if (buf[i] == '\"' && name_off == 0) {
        name_off = i + 1;
      } else if (buf[i] == '\"') {
        buf[i] = '\0';
        strcpy_s(new_inst.shush_name, buf + name_off);
        i += 3;
        break;
      }
    }

    int opcode;
    sscanf_s(buf + i, "%x", &opcode);
    offset = 7 + i;
    new_inst.shush_opcode = opcode;

    if (buf[offset] == ']') {
      new_inst.shush_args.type = inst::ShushArgs::Type::NO_TYPE;
      new_inst.shush_args.size = 0;
    } else {
      char* type_name = nullptr;
      name_off = 0;
      for (i = offset; ; ++i) {
        if (buf[i] == '\"' && name_off == 0) {
          name_off = i + 1;
        } else if (buf[i] == '\"') {
          buf[i] = '\0';
          type_name = buf + name_off;
          break;
        }
      }

      if (!strcmp(type_name, "double")) {
        new_inst.shush_args.type = inst::ShushArgs::Type::DOUBLE;
        new_inst.shush_args.size = sizeof(double);
      } else if (!strcmp(type_name, "register")) {
        new_inst.shush_args.type = inst::ShushArgs::Type::REGISTER;
        new_inst.shush_args.size = sizeof(uint8_t);
      } else if (!strcmp(type_name, "label")) {
        new_inst.shush_args.type = inst::ShushArgs::Type::ADDRESS;
        new_inst.shush_args.size = sizeof(uint64_t);
      } else {
        assert(0);
      }
    }

    inst_info.PushBack(new_inst);
  }

  fclose(inst_file);
}

shush::pe_trans::TranslationError shush::pe_trans::Translator::FirstPass() {
  fseek(in,  0, SEEK_SET);
  fseek(out, detail::TEMPLATE_PE32_CODE_START, SEEK_SET);

  const size_t shush_file_size = GetFileSize(in);

  for (; !IsEndOfShushFile();) {
    const size_t cur_shush_byte = ftell(in);

    auto inst = ParseCurrentShushInstruction();
    fseek(in, inst.size_in, SEEK_CUR);

    if (write_mode == detail::TranslationWritingMode::WRITE) {
      fwrite(inst.out_res, sizeof(char), inst.size_out, out);
    } else  if (write_mode == detail::TranslationWritingMode::APPEND) {
      //TODO
    }
  }

  return TranslationError::NO_ERROR;
}

shush::pe_trans::TranslationError shush::pe_trans::Translator::SecondPass() {
  addrs_to_fill.Sort();

  size_t cur_addr_info = 0;
  const size_t addrs_info_size = addrs_to_fill.GetSize();

  fseek(in,  0, SEEK_SET);
  fseek(out, detail::TEMPLATE_PE32_CODE_START, SEEK_SET);

  const size_t shush_file_size = GetFileSize(in);

  for (; !IsEndOfShushFile(); IncrementFileOffsets()) {
    if (cur_addr_info == addrs_info_size) {
      break;
    }

    const size_t cur_shush_byte = ftell(in);

    while (addrs_to_fill[cur_addr_info].in_file_com_byte == cur_shush_byte
           && cur_addr_info < addrs_info_size) {
      FillRva(addrs_to_fill[cur_addr_info].out_file_rva_byte);
      ++cur_addr_info;
    }
  }

  return TranslationError::NO_ERROR;
}

void shush::pe_trans::Translator::FillRva(const size_t where) const {
  const uint32_t pos = ftell(out);
  const uint32_t rva = pos + detail::DEFAULT_BASE_RVA;

  fseek(out, where, SEEK_SET);
  fwrite(&rva, sizeof(rva), 1, out);
  fseek(out, pos, SEEK_SET);
}

bool shush::pe_trans::Translator::IsEndOfShushFile() const {
  return ftell(in) == GetFileSize(in);
}

void shush::pe_trans::Translator::IncrementFileOffsets() {
  const auto inst = ParseCurrentShushInstruction();
  fseek(in, inst.size_in, SEEK_CUR);
  fseek(out, inst.size_out, SEEK_CUR);
}

shush::pe_trans::inst::ShushInstructionConverted
shush::pe_trans::Translator::ConvertInstruction(
  inst::ShushInstructionInfo* info) const {
  inst::ShushInstructionConverted inst;

  inst.size_in = sizeof(char) + info->shush_args.size;

  if (mode == TranslationMode::TRANSLATE_TO_ASM_CODE) {

    inst.size_out = strlen(inst.out_res);
    return inst;
  }

  switch (info->shush_opcode) {
    case 0x30 :
      { // in
        memcpy_s(inst.out_res, inst::MAX_INSTRUCTION_SIZE,
                 IN_INSTRUCTION, sizeof(IN_INSTRUCTION));
        inst.size_out = sizeof(IN_INSTRUCTION);
      } break;
    case 0x31 :
      { // out
        memcpy_s(inst.out_res, inst::MAX_INSTRUCTION_SIZE,
                 OUT_INSTRUCTION, sizeof(OUT_INSTRUCTION));
        inst.size_out = sizeof(OUT_INSTRUCTION);
      } break;

    default: assert(0);
  }

  
  return inst;
}

shush::pe_trans::inst::ShushInstructionConverted
shush::pe_trans::Translator::ParseCurrentShushInstruction() {
  const auto prev_pos = ftell(in);

  char opcode = 0;
  fread(&opcode, sizeof(char), 1, in);

  auto* this_inst_info = FindShushInstructionInfoByOpcode(opcode);
  inst::ShushInstructionInfo this_inst_copy_with_args = *this_inst_info;

  const auto type = this_inst_info->shush_args.type;
  auto& value = this_inst_copy_with_args.shush_args.value;

  switch (type) {
    case inst::ShushArgs::Type::NO_TYPE :
      {
        
      } break;
    case inst::ShushArgs::Type::DOUBLE :
      {
        fread_s(&value, sizeof(double), sizeof(double), 1, in);
      } break;
    case inst::ShushArgs::Type::ADDRESS :
      {
        fread_s(&value, sizeof(uint64_t), sizeof(uint64_t), 1, in);
      }
    case inst::ShushArgs::Type::REGISTER :
      {
        fread_s(&value, sizeof(uint8_t), sizeof(uint8_t), 1, in);
      }

    default : break;
  }

  const inst::ShushInstructionConverted inst =
    ConvertInstruction(&this_inst_copy_with_args);

  fseek(in, prev_pos, SEEK_SET);

  return inst;
}

shush::pe_trans::inst::ShushInstructionInfo* shush::pe_trans::Translator::
FindShushInstructionInfoByOpcode(char opcode) {
  for (size_t i = 0, size = inst_info.GetSize(); i < size; ++i) {
    if (inst_info[i].shush_opcode == opcode) {
      return &inst_info[i];
    }
  }
  assert(0);
  return nullptr;
}
