#!/usr/bin/env python3

import op

def gen_prototype(opinfo, extra):
    name, param, pop, push = opinfo

    ps = []
    if param == op.PARAM_U8:
        ps.append("uint8_t dp0")
    elif param == op.PARAM_I16:
        ps.append("int16_t dp0")
    elif param == op.PARAM_I32:
        ps.append("int32_t dp0")
    elif param == op.PARAM_STR:
        ps.append("const std::vector<uint8_t> &dp0")
    elif param == op.PARAM_ADDR:
        ps.append("uint32_t dp0")
    elif param == op.PARAM_ADDR_U8:
        ps.append("uint32_t dp0")
        ps.append("uint8_t dp1")
    elif param == op.PARAM_JMP:
        ps.append("uint32_t dp0")
    elif param == op.PARAM_PRESET:
        ps.append("uint32_t dp0")
        ps.append("const std::vector<uint8_t> &dp2")

    for i in range(pop):
        ps.append(f"uint32_t ds{i}")

    ret = "uint32_t" if push else "void"

    return f"""
{ret} {extra}lava_op_{name}({", ".join(ps)})
""".strip()

def gen_declaration(file, opcode, opinfo):
    name, param, pop, push = opinfo

    file.write(f"""
void lava_wrap_{name}(const std::vector<uint8_t> &data);
inline {gen_prototype(opinfo, "")};
""")

def gen_definition(file, opcode, opinfo):
    name, param, pop, push = opinfo

    lines = []
    ps = []

    lines.append(f'    DEBUG_WRAP_PRINT("{name}(");')

    if param == op.PARAM_U8:
        lines.append("    uint8_t dp0 = data[0];")
        lines.append("    DEBUG_WRAP_PRINT((uint32_t)dp0);")
        ps.append("dp0")
    elif param == op.PARAM_I16:
        lines.append("    int16_t dp0 = (int16_t)(data[0] | (data[1] << 8));")
        lines.append("    DEBUG_WRAP_PRINT(dp0);")
        ps.append("dp0")
    elif param == op.PARAM_I32:
        lines.append("    int32_t dp0 = (int32_t)(data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24));")
        lines.append("    DEBUG_WRAP_PRINT(dp0);")
        ps.append("dp0")
    elif param == op.PARAM_STR:
        lines.append("    const std::vector<uint8_t> &dp0 = data;")
        lines.append("    DEBUG_WRAP_PRINT('<' << dp0.size() << '>');")
        ps.append("dp0")
    elif param == op.PARAM_ADDR:
        lines.append("    uint32_t dp0;")
        lines.append("    if (rambits <= 16) {")
        lines.append("        dp0 = (uint32_t)(data[0] | (data[1] << 8));")
        lines.append("    } else {")
        lines.append("        dp0 = (uint32_t)(data[0] | (data[1] << 8) | (data[2] << 16));")
        lines.append("    }")
        lines.append("    DEBUG_WRAP_PRINT(dp0);")
        ps.append("dp0")
    elif param == op.PARAM_ADDR_U8:
        lines.append("    uint32_t dp0;")
        lines.append("    uint8_t dp1;")
        lines.append("    if (rambits <= 16) {")
        lines.append("        dp0 = (uint32_t)(data[0] | (data[1] << 8));")
        lines.append("        dp1 = data[2];")
        lines.append("    } else {")
        lines.append("        dp0 = (uint32_t)(data[0] | (data[1] << 8) | (data[2] << 16));")
        lines.append("        dp1 = data[3];")
        lines.append("    }")
        lines.append("    DEBUG_WRAP_PRINT(dp0);")
        lines.append("    DEBUG_WRAP_PRINT(\", \" << (uint32_t)dp1);")
        ps.append("dp0")
        ps.append("dp1")
    elif param == op.PARAM_JMP:
        lines.append("    uint32_t dp0 = (uint32_t)(data[0] | (data[1] << 8) | (data[2] << 16));")
        lines.append("    DEBUG_WRAP_PRINT(dp0);")
        ps.append("dp0")
    elif param == op.PARAM_PRESET:
        lines.append("    uint32_t dp0;")
        lines.append("    uint16_t dp1;")
        lines.append("    std::vector<uint8_t> dp2;")
        lines.append("    if (rambits <= 16) {")
        lines.append("        dp0 = (uint32_t)(data[0] | (data[1] << 8));")
        lines.append("        dp1 = data[2] | (data[3] << 8);")
        lines.append("        dp2 = std::vector<uint8_t>(data.cbegin() + 4, data.cend());")
        lines.append("    } else {")
        lines.append("        dp0 = (uint32_t)(data[0] | (data[1] << 8) | (data[2] << 16));")
        lines.append("        dp1 = data[3] | (data[4] << 8);")
        lines.append("        dp2 = std::vector<uint8_t>(data.cbegin() + 5, data.cend());")
        lines.append("    }")
        lines.append("    DEBUG_WRAP_PRINT(dp0);")
        lines.append("    DEBUG_WRAP_PRINT(\", \" << dp1);")
        lines.append("    DEBUG_WRAP_PRINT(\", <\" << dp2.size() << '>');")
        ps.append("dp0")
        ps.append("dp2")

    pstack = []
    for i in range(pop):
        lines.append(f"    uint32_t ds{i} = ram.pop();")
        if (i != 0 or param != op.PARAM_NONE):
            lines.append("    DEBUG_WRAP_PRINT(\", \");")
        lines.append(f"    DEBUG_WRAP_PRINT(ds{i});")
        pstack.append(f"ds{i}")

    lines.append('    DEBUG_WRAP_PRINT(")");')

    ret = "uint32_t ret = " if push else ""
    if not ret:
        lines.append('    DEBUG_WRAP_PRINT(std::endl);')

    lines.append("    pc += 1 + data.size();")
    lines.append(f"""    {ret}lava_op_{name}({", ".join(ps + pstack)});""")
    if push:
        lines.append("    ram.push(ret);")
    if ret:
        lines.append('    DEBUG_WRAP_PRINT(" -> " << ret << std::endl);')

    lines = "\n".join(lines)

    file.write(f"""
void LavaProc::lava_wrap_{name}(const std::vector<uint8_t> &data)
{{
{lines}
}}
""")

def gen_template(opcode, opinfo):
    name, param, pop, push = opinfo

    lines = []
    lines.append("    TODO();")
    if push:
        lines.append("    return 0;")
    lines = "\n".join(lines)

    print(f"""
{gen_prototype(opinfo, "LavaProc::")}
{{
{lines}
}}""")

def gen_map(opcode, opinfo):
    name, param, pop, push = opinfo

    plen = "OpParam0"
    if param == op.PARAM_U8:
        plen = "OpParam1"
    elif param == op.PARAM_I16:
        plen = "OpParam2"
    elif param == op.PARAM_I32:
        plen = "OpParam4"
    elif param == op.PARAM_STR:
        plen = "(op_len_t)(OpParam0 | OpParamStr)"
    elif param == op.PARAM_ADDR:
        plen = "(op_len_t)(OpParam0 | OpParamAddr)"
    elif param == op.PARAM_ADDR_U8:
        plen = "(op_len_t)(OpParam1 | OpParamAddr)"
    elif param == op.PARAM_JMP:
        plen = "OpParam3"
    elif param == op.PARAM_PRESET:
        plen = "(op_len_t)(OpParam2 | OpParamAddr | OpParamPreset)"

    return f'    {{{opcode:#04x}, op_info_t{{"{name}", {plen}, &LavaProc::lava_wrap_{name}}}}},'

def main():
    fheader = open("core/lava_op_defs.h", "w")
    fcode = open("core/lava_op_code.h", "w")

    op_map = []
    names = set()
    for opcode, opinfo in op.op.items():
        name, param, pop, push = opinfo

        if name in names:
            # Duplicated opcode
            op_map.append(gen_map(opcode, opinfo))
            continue
        names.add(name)

        gen_declaration(fheader, opcode, opinfo)
        gen_definition(fcode, opcode, opinfo)
        gen_template(opcode, opinfo)
        op_map.append(gen_map(opcode, opinfo))

    op_map = "\n".join(op_map)
    fcode.write(f"""
std::unordered_map<uint8_t, LavaProc::op_info_t> LavaProc::op_info = {{
{op_map}
}};
""")

main()
