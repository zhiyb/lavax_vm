#!/usr/bin/env python3

ram_size = 16

def op_str_size(data, ofs):
    i = 0
    # TODO secret XOR
    while data[ofs + i] != 0:
        i += 1
    return i + 1

def op_str_data(d, ofs = 1):
    return f'"{d[ofs:].decode("GB2312")}"'

def op_data_i16(d):
    return d[0] + (d[1] << 8) - (0x10000 if d[1] & 0x80 else 0)

def op_data_u24(d):
    return d[0] + (d[1] << 8) + (d[2] << 16)

def op_data_u32(d):
    return d[0] + (d[1] << 8) + (d[2] << 16) + (d[3] << 24)

def op_vaddr(d):
    return d[0] + (d[1] << 8) + (d[2] << 16 if ram_size > 16 else 0)

def op_vaddr_size():
    return 3 if ram_size > 16 else 2

op_decode = {
    0x01: ("push_char", 2, lambda d: d[1]),
    0x02: ("push_int", 3, lambda d: op_data_i16(d[1:3])),
    0x03: ("push_long", 5, lambda d: op_data_u32(d[1:5])),
    0x04: ("push_vchar",
           lambda _, __: 1 + op_vaddr_size(),
           lambda d: f"*{op_vaddr(d[1:]):#x}"),
    0x05: ("push_vint",
           lambda _, __: 1 + op_vaddr_size(),
           lambda d: f"*{op_vaddr(d[1:]):#x}"),
    0x06: ("push_vlong",
           lambda _, __: 1 + op_vaddr_size(),
           lambda d: f"*{op_vaddr(d[1:]):#x}"),
    0x07: ("push_gchar",
           lambda _, __: 1 + op_vaddr_size(),
           lambda d: f"*{op_vaddr(d[1:]):#x}"),
    0x08: ("push_gint",
           lambda _, __: 1 + op_vaddr_size(),
           lambda d: f"*{op_vaddr(d[1:]):#x}"),
    0x09: ("push_glong",
           lambda _, __: 1 + op_vaddr_size(),
           lambda d: f"*{op_vaddr(d[1:]):#x}"),
    0x0a: ("push_achar",
           lambda _, __: 1 + op_vaddr_size(),
           lambda d: f"*{op_vaddr(d[1:]):#x}"),
    0x0d: ("push_string",
           lambda data, ofs: 1 + op_str_size(data, ofs + 1),
           lambda d: op_str_data(d, 1)),
    0x17: ("push_along",
           lambda _, __: 1 + op_vaddr_size(),
           lambda d: f"*{op_vaddr(d[1:]):#x}"),
    0x1f: ("INC", 1),
    0x20: ("DEC", 1),
    0x21: ("add", 1),
    0x22: ("sub", 1),
    0x27: ("land", 1),
    0x28: ("lor", 1),
    0x2a: ("mul", 1),
    0x2b: ("div", 1),
    0x2f: ("equ", 1),
    0x31: ("le", 1),
    0x32: ("ge", 1),
    0x33: ("great", 1),
    0x34: ("less", 1),
    0x35: ("let", 1), #9, lambda d: f"{op_data_u32(d[1:5])}, {op_data_u32(d[5:9])}"),
    0x38: ("pop_val", 1),
    0x39: ("jmpe", 4, lambda d: f"{op_data_u24(d[1:4]):#x}"),
    0x3b: ("jmp", 4, lambda d: f"{op_data_u24(d[1:4]):#x}"),
    0x3c: ("set_sp",
           lambda _, __: 1 + op_vaddr_size(),
           lambda d: f"{op_vaddr(d[1:]):#x}"),
    0x3e: ("add_bp",
           lambda _, __: 2 + op_vaddr_size(),
           lambda d: f"{op_vaddr(d[1:]):#x}, {d[1 + op_vaddr_size()]}"),
    0x40: ("good_exit", 1),
    0x45: ("qadd", 3, lambda d: op_data_i16(d[1:3])),
    0x46: ("qsub", 3, lambda d: op_data_i16(d[1:3])),
    0x47: ("qmul", 3, lambda d: op_data_i16(d[1:3])),
    0x48: ("qdiv", 3, lambda d: op_data_i16(d[1:3])),
    0x4a: ("qlshift", 3, lambda d: op_data_i16(d[1:3])),
    0x4c: ("qequ", 3, lambda d: op_data_i16(d[1:3])),
    0x4d: ("qneq", 3, lambda d: op_data_i16(d[1:3])),
    0x4e: ("qgreat", 3, lambda d: op_data_i16(d[1:3])),
    0x4f: ("qless", 3, lambda d: op_data_i16(d[1:3])),
    0x81: ("getchar", 1),
    0x84: ("strlen", 1),
    0x87: ("delay", 1),
    0x88: ("writeblock", 1),
    0x89: ("scroll_to_lcd", 1),
    0x8a: ("textout", 1),
    0x8b: ("block", 1),
    0x8c: ("rectangle", 1),
    0x8d: ("exit", 1),
    0x8e: ("clearscreen", 1),
    0xae: ("fopen", 1),
    0xaf: ("fclose", 1),
    0xb0: ("fread", 1),
    0xb1: ("fwrite", 1),
    0xb8: ("sprintf", 1),
    0xbc: ("checkkey", 1),
    0xcb: ("setgraphmode", 1),
    0xcc: ("setbgcolor", 1),
    0xcd: ("setfgcolor", 1),
}

op_str = [
	"sn_err","push_char","push_int","push_long","push_vchar","push_vint","push_vlong",
	"push_gchar","push_gint","push_glong","push_achar","push_aint","push_along","push_string",
	"push_lvchar","push_lvint","push_lvlong","push_lgchar","push_lgint","push_lglong",
	"push_lachar","push_laint","push_lalong","push_along","push_lalong","push_llong",
	"push_text","push_graph","push_sub0","cal_inc","cal_dec","cal_INC","cal_DEC",
	"cal_add","cal_sub","cal_and","cal_or","push_not","cal_xor","cal_land","cal_lor","cal_lnot",
	"cal_mul","cal_div","cal_mod","cal_lshift","cal_rshift","cal_equ","cal_neq","cal_le","cal_ge","cal_great","cal_less",
	"c_let","c_ptr","c_cptr","pop_val","c_jmpe","c_jmpn","c_jmp","set_sp","c_call","add_bp","sub_bp",
	"good_exit","c_preset","push_gbuf","c_secret","c_loadall",
	"cal_qadd","cal_qsub","cal_qmul","cal_qdiv","cal_qmod","cal_qlshift","cal_qrshift",
	"cal_qequ","cal_qneq","cal_qgreat","cal_qless","cal_qge","cal_qle",
	"c_iptr","c_lptr",
	"c_icf","c_fci","cal_addff","cal_addf","cal_add0f","cal_subff","cal_subf","cal_sub0f",
	"cal_mulff","cal_mulf","cal_mul0f","cal_divff","cal_divf","cal_div0f","push_sub0f",
	"cal_lessf","cal_greatf","cal_equf","cal_neqf","cal_lef","cal_gef","c_f0",
	"c_ciptr","c_clptr","c_lcc","c_lci","c_letx","push_ax","cal_idx","c_pass","c_void",
	"c_debug","c_funcid"
]

op2_str = [
    "c_putchar","c_getchar","c_printf","c_strcpy","c_strlen","c_setscreen","c_updatelcd",
	"c_delay","c_writeblock","scroll_to_lcd","c_textout","c_block","c_rectangle",
	"c_exit","c_clearscreen","c_abs","c_rand","c_srand","c_locate","c_inkey","c_point",
	"c_getpoint","c_line","c_box","c_circle","c_ellipse","c_beep","c_isalnum","c_isalpha",
	"c_iscntrl","c_isdigit","c_isgraph","c_islower","c_isprint","c_ispunct","c_isspace",
	"c_isupper","c_isxdigit","c_strcat","c_strchr","c_strcmp","c_strstr","c_tolower",
	"c_toupper","c_memset","c_memcpy","c_fopen","c_fclose","c_fread","c_fwrite",
	"c_fseek","c_ftell","c_feof","c_rewind","c_getc","c_putc","c_sprintf","c_makedir",
	"c_deletefile","c_getms","c_checkkey","c_memmove","c_crc16","c_jiami","c_chdir",
	"c_filelist","c_gettime","c_settime","c_getword","c_xdraw","c_releasekey","c_getblock",
	"c_sin","c_cos","c_fill","c_setgraphmode","c_setbgcolor","c_setfgcolor",
	"c_setlist","c_fade","c_exec","c_findfile","c_getfilenum","c_system","c_math",
	"c_setpalette","c_getcmdline"
]

def parse_op(data, ofs):
    op = data[ofs]
    size = 1

    if op in op_decode:
        op_info = op_decode[op]
        size = op_info[1]
        if callable(size):
            size = size(data, ofs)

        op_data = data[ofs : ofs+size]
        op_data_hex = [f"{v:02x}" for v in op_data]
        op_params = op_info[2](op_data) if len(op_info) > 2 else ""
        print(f"{op_info[0]}({op_params}) - {op_data_hex}")

    else:
        op_name = op2_str[op & 0x7f] if op & 0x80 else op_str[op]
        raise Exception(f"Unknown OP code at {ofs:#010x}: {op:#04x} - {op_name}")

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
