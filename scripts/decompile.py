#!/usr/bin/env python3

import op

ram_size = 16

def parse_op(data, ofs):
    opcode = data[ofs]

    op_info = op.op.get(opcode, None)
    if op_info is None:
        raise Exception(f"Unknown OP code at {ofs:#010x}: {opcode:#04x} - {op.op_str[opcode]}")

    op_info = op.op[opcode]
    name, ptype, pop, push = op_info

    size = op.op_psize[ptype]
    if callable(size):
        size = size(data, ofs, ram_size)
    size += 1

    op_data = data[ofs : ofs+size]
    op_data_hex = [f"{v:02x}" for v in op_data]
    op_params = op.op_pdata[ptype](op_data[1:], ram_size)
    print(f"{name}({op_params}) - {op_data_hex}")

    return size

def parse(data):
    header = data[0:16]
    print(f"File header: {header}")

    h_sig = header[0:4]
    if h_sig == b'LAV\x12':
        print(f"File signature: OK")
    else:
        print(f"File signature: FAIL {h_sig}")

    h_cfg = header[8]
    ram_size = {0x00: 16, 0x10: 32, 0x80: 24}[h_cfg & 0x90]
    print(f"RAM size: {ram_size}")
    graphic_mode = {0x00: 1, 0x40: 4, 0x60: 8}[h_cfg & 0x60]
    graphic_name = {1: "Monochromatic", 4: "16-colour", 8: "256-colour"}[graphic_mode]
    print(f"Graphic mode: {graphic_name}")

    h_size = header[9:11]
    width = h_size[0] * 16
    width = 160 if width < 160 else width
    width = 320 if width > 320 else width
    height = h_size[1] * 16
    height = 80 if height < 80 else height
    height = 240 if height > 240 else height
    print(f"Window size: {width}x{height}")

    ofs = 16
    while ofs < len(data):
        ofs += parse_op(data, ofs)


def main():
    with open("test.lav", "rb") as flav:
        data = flav.read()
        parse(data)

main()
