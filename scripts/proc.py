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

    for i in range(pop):
        ps.append(f"uint32_t ds{i}")

    return f"""
{"uint32_t" if push else "void"} {extra}lava_op_{name}({", ".join(ps)})
""".strip()

def gen_declaration(file, opcode, opinfo):
    name, param, pop, push = opinfo

    file.write(f"""
void lava_wrap_{name}(const std::vector<uint8_t> &data);
{gen_prototype(opinfo, "")};
""")

def gen_definition(file, opcode, opinfo):
    name, param, pop, push = opinfo

    pvars = []
    ps = []
    if param == op.PARAM_U8:
        pvars.append("    uint8_t dp0 = data[0];")
        ps.append("dp0")
    elif param == op.PARAM_I16:
        pvars.append("    int16_t dp0 = (int16_t)(data[0] | (data[1] << 8));")
        ps.append("dp0")
    elif param == op.PARAM_I32:
        pvars.append("    int32_t dp0 = (int32_t)(data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24));")
        ps.append("dp0")
    elif param == op.PARAM_STR:
        pvars.append("    const std::vector<uint8_t> &dp0 = data;")
        ps.append("dp0")
    elif param == op.PARAM_ADDR:
        pvars.append("    uint32_t dp0;")
        pvars.append("    if (rambits <= 16) {")
        pvars.append("        dp0 = (uint32_t)(data[0] | (data[1] << 8));")
        pvars.append("    } else {")
        pvars.append("        dp0 = (uint32_t)(data[0] | (data[1] << 8) | (data[2] << 16));")
        pvars.append("    }")
        ps.append("dp0")
    elif param == op.PARAM_ADDR_U8:
        pvars.append("    uint32_t dp0;")
        pvars.append("    uint8_t dp1;")
        pvars.append("    if (rambits <= 16) {")
        pvars.append("        dp0 = (uint32_t)(data[0] | (data[1] << 8));")
        pvars.append("        dp1 = data[2];")
        pvars.append("    } else {")
        pvars.append("        dp0 = (uint32_t)(data[0] | (data[1] << 8) | (data[2] << 16));")
        pvars.append("        dp1 = data[3];")
        pvars.append("    }")
        ps.append("dp0")
        ps.append("dp1")
    elif param == op.PARAM_JMP:
        pvars.append("    uint32_t dp0 = (uint32_t)(data[0] | (data[1] << 8) | (data[2] << 16));")
        ps.append("dp0")

    pstack = []
    for i in range(pop):
        pvars.append(f"    uint32_t ds{i} = ram.pop();")
        pstack.append(f"ds{i}")

    pvars = "\n".join(pvars)

    file.write(f"""
void LavaProc::lava_wrap_{name}(const std::vector<uint8_t> &data)
{{
{pvars}
    {"uint32_t ret = " if push else ""}lava_op_{name}({", ".join(ps + pstack)});
    {"ram.push(ret);" if push else ";"}
}}
""")

def gen_template(opcode, opinfo):
    name, param, pop, push = opinfo
    print(f"""
{gen_prototype(opinfo, "LavaProc::")}
{{
    {"return 0;" if push else ";"}
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
        plen = "OpParamStr"
    elif param == op.PARAM_ADDR:
        plen = "(op_len_t)(OpParam0 | OpParamAddr)"
    elif param == op.PARAM_ADDR_U8:
        plen = "(op_len_t)(OpParam1 | OpParamAddr)"
    elif param == op.PARAM_JMP:
        plen = "OpParam3"

    return f'    {{{opcode:#04x}, op_info_t{{"{name}", {plen}, &LavaProc::lava_wrap_{name}}}}},'

def main():
    fheader = open("core/lava_op_defs.h", "w")
    fcode = open("core/lava_op_code.h", "w")

    op_map = []
    for opcode, opinfo in op.op.items():
        gen_declaration(fheader, opcode, opinfo)
        gen_definition(fcode, opcode, opinfo)
        gen_template(opcode, opinfo)
        op_map.append(gen_map(opcode, opinfo))

    op_map = "\n".join(op_map)
    fcode.write(f"""
std::map<uint8_t, LavaProc::op_info_t> LavaProc::op_info = {{
{op_map}
}};
""")

main()
