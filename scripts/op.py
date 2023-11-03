#!/usr/bin/env python3

PARAM_NONE    = 0x00   # 0 bytes
PARAM_U8      = 0x01   # 1 byte
PARAM_I16     = 0x02   # 2 bytes
PARAM_I32     = 0x03   # 4 bytes
PARAM_STR     = 0x04   # variable
PARAM_ADDR    = 0x05   # 2 or 3 bytes
PARAM_ADDR_U8 = 0x06   # 3 or 4 bytes
PARAM_JMP     = 0x07   # 3 bytes
PARAM_PRESET  = 0x08   # variable

op = {
    # OP:  Name,         params,     pop, push
    0x01: ("push_u8",      PARAM_U8,      0, 1),
    0x02: ("push_i16",     PARAM_I16,     0, 1),
    0x03: ("push_i32",     PARAM_I32,     0, 1),
    0x04: ("pushv_u8",     PARAM_ADDR,    0, 1),
    0x05: ("pushv_i16",    PARAM_ADDR,    0, 1),
    0x06: ("pushv_i32",    PARAM_ADDR,    0, 1),
    0x07: ("pushg_u8",     PARAM_ADDR,    1, 1),
    0x08: ("pushg_i16",    PARAM_ADDR,    1, 1),
    0x09: ("pushg_i32",    PARAM_ADDR,    1, 1),
    0x0a: ("pusha_u8",     PARAM_ADDR,    1, 1),
    0x0b: ("pusha_i16",    PARAM_ADDR,    1, 1),
    0x0c: ("pusha_i32",    PARAM_ADDR,    1, 1),
    0x0d: ("push_str",     PARAM_STR,     0, 1),
    0x0e: ("pushlv_u8",    PARAM_ADDR,    0, 1),
    0x0f: ("pushlv_i16",   PARAM_ADDR,    0, 1),
    0x10: ("pushlv_i32",   PARAM_ADDR,    0, 1),
    0x11: ("pushlg_char",  PARAM_ADDR,    1, 1),
    0x14: ("pushla_u8",    PARAM_ADDR,    1, 1),
    0x16: ("pushla_i32",   PARAM_ADDR,    1, 1),
    0x17: ("pusha_i32",    PARAM_ADDR,    1, 1),
    0x19: ("pushl_i32",    PARAM_ADDR,    0, 1),
    0x1d: ("pre_inc",      PARAM_NONE,    1, 1),
    0x1e: ("pre_dec",      PARAM_NONE,    1, 1),
    0x1f: ("post_inc",     PARAM_NONE,    1, 1),
    0x20: ("post_dec",     PARAM_NONE,    1, 1),
    0x21: ("add",          PARAM_NONE,    2, 1),
    0x22: ("sub",          PARAM_NONE,    2, 1),
    0x23: ("and",          PARAM_NONE,    2, 1),
    0x24: ("or",           PARAM_NONE,    2, 1),
    0x26: ("xor",          PARAM_NONE,    2, 1),
    0x27: ("land",         PARAM_NONE,    2, 1),
    0x28: ("lor",          PARAM_NONE,    2, 1),
    0x2a: ("mul",          PARAM_NONE,    2, 1),
    0x2b: ("div",          PARAM_NONE,    2, 1),
    0x2c: ("mod",          PARAM_NONE,    2, 1),
    0x2f: ("equ",          PARAM_NONE,    2, 1),
    0x30: ("neq",          PARAM_NONE,    2, 1),
    0x31: ("le",           PARAM_NONE,    2, 1),
    0x32: ("ge",           PARAM_NONE,    2, 1),
    0x33: ("great",        PARAM_NONE,    2, 1),
    0x34: ("less",         PARAM_NONE,    2, 1),
    0x35: ("let",          PARAM_NONE,    2, 1),
    0x36: ("ptr",          PARAM_NONE,    1, 1),
    0x38: ("pop",          PARAM_NONE,    1, 0),
    0x39: ("jmpe",         PARAM_JMP,     0, 0),
    0x3a: ("jmpn",         PARAM_JMP,     0, 0),
    0x3b: ("jmp",          PARAM_JMP,     0, 0),
    0x3c: ("set_sp",       PARAM_ADDR,    0, 0),
    0x3d: ("call",         PARAM_JMP,     0, 0),
    0x3e: ("add_bp",       PARAM_ADDR_U8, 0, 0),
    0x3f: ("sub_bp",       PARAM_NONE,    0, 0),
    0x40: ("quit",         PARAM_NONE,    0, 0),
    0x41: ("preset",       PARAM_PRESET,  0, 0),
    0x45: ("qadd",         PARAM_I16,     1, 1),
    0x46: ("qsub",         PARAM_I16,     1, 1),
    0x47: ("qmul",         PARAM_I16,     1, 1),
    0x48: ("qdiv",         PARAM_I16,     1, 1),
    0x49: ("qmod",         PARAM_I16,     1, 1),
    0x4a: ("qlshift",      PARAM_I16,     1, 1),
    0x4b: ("qrshift",      PARAM_I16,     1, 1),
    0x4c: ("qequ",         PARAM_I16,     1, 1),
    0x4d: ("qneq",         PARAM_I16,     1, 1),
    0x4e: ("qgreat",       PARAM_I16,     1, 1),
    0x4f: ("qless",        PARAM_I16,     1, 1),
    0x50: ("qge",          PARAM_I16,     1, 1),
    0x51: ("qle",          PARAM_I16,     1, 1),
    0x81: ("getchar",      PARAM_NONE,    0, 1),
    0x82: ("printf",       PARAM_NONE,    1, 0),
    0x83: ("strcpy",       PARAM_NONE,    2, 0),
    0x84: ("strlen",       PARAM_NONE,    1, 1),
    0x85: ("setscreen",    PARAM_NONE,    1, 0),
    0x87: ("delay",        PARAM_NONE,    1, 0),
    0x88: ("writeblock",   PARAM_NONE,    6, 0),
    0x89: ("fbswap",       PARAM_NONE,    0, 0),
    0x8a: ("textout",      PARAM_NONE,    4, 0),
    0x8b: ("block",        PARAM_NONE,    5, 0),
    0x8c: ("rectangle",    PARAM_NONE,    5, 0),
    0x8d: ("exit",         PARAM_NONE,    1, 0),
    0x8e: ("clearscreen",  PARAM_NONE,    0, 0),
    0x90: ("rand",         PARAM_NONE,    0, 1),
    0x91: ("srand",        PARAM_NONE,    1, 0),
    0x92: ("locate",       PARAM_NONE,    2, 0),
    0x93: ("inkey",        PARAM_NONE,    0, 1),
    0x94: ("point",        PARAM_NONE,    3, 0),
    0x96: ("line",         PARAM_NONE,    5, 0),
    0x98: ("circle",       PARAM_NONE,    5, 0),
    0xa7: ("strchr",       PARAM_NONE,    2, 1),
    0xa8: ("strcmp",       PARAM_NONE,    2, 1),
    0xa9: ("strstr",       PARAM_NONE,    2, 1),
    0xac: ("memset",       PARAM_NONE,    3, 0),
    0xad: ("memcpy",       PARAM_NONE,    3, 0),
    0xae: ("fopen",        PARAM_NONE,    2, 1),
    0xaf: ("fclose",       PARAM_NONE,    1, 0),
    0xb0: ("fread",        PARAM_NONE,    4, 1),
    0xb1: ("fwrite",       PARAM_NONE,    4, 1),
    0xb2: ("fseek",        PARAM_NONE,    3, 1),
    0xb5: ("rewind",       PARAM_NONE,    1, 0),
    0xb8: ("sprintf",      PARAM_NONE,    1, 0),
    0xbb: ("gettick",      PARAM_NONE,    0, 1),
    0xbc: ("checkkey",     PARAM_NONE,    1, 1),
    0xc6: ("releasekey",   PARAM_NONE,    1, 0),
    0xc7: ("getblock",     PARAM_NONE,    6, 0),
    0xcb: ("setgraphmode", PARAM_NONE,    1, 1),
    0xcc: ("setbgcolor",   PARAM_NONE,    1, 0),
    0xcd: ("setfgcolor",   PARAM_NONE,    1, 0),
}


op1_str = [
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

op_str = {}

for i,name in enumerate(op1_str):
    op_str[i] = name
for i,name in enumerate(op2_str):
    op_str[i + 0x80] = name


def op_str_size(data, ofs, rambits):
    i = 0
    # TODO secret XOR
    while data[ofs + i] != 0:
        i += 1
    return i + 1

def op_addr_size(data, ofs, rambits):
    return 3 if rambits > 16 else 2

def op_addrd_size(data, ofs, rambits):
    return 4 if rambits > 16 else 3

def op_preset_size(data, ofs, rambits):
    size = op_addr_size(data, ofs, rambits)
    dlen = data[ofs + size] + (data[ofs + size + 1] << 8)
    size += 2 + dlen
    return size

op_psize = {
    PARAM_NONE:    0,
    PARAM_U8:      1,
    PARAM_I16:     2,
    PARAM_I32:     4,
    PARAM_STR:     op_str_size,
    PARAM_ADDR:    op_addr_size,
    PARAM_ADDR_U8: op_addrd_size,
    PARAM_JMP:     3,
    PARAM_PRESET:  op_preset_size,
}

def op_vaddr(d, ram_size):
    return d[0] + (d[1] << 8) + (d[2] << 16 if ram_size > 16 else 0)

def op_vaddr_size(ram_size):
    return 3 if ram_size > 16 else 2

op_pdata = {
    PARAM_NONE:    lambda d, ram_size: "",
    PARAM_U8:      lambda d, ram_size: f"{d[0]:#x}",
    PARAM_I16:     lambda d, ram_size: f"{d[0] + (d[1] << 8) - (0x10000 if d[1] & 0x80 else 0):#x}",
    PARAM_I32:     lambda d, ram_size: f"{d[0] + (d[1] << 8) + (d[2] << 16) + (d[3] << 24) - (0x100000000 if d[3] & 0x80 else 0):#x}",
    PARAM_STR:     lambda d, ram_size: f'"{d.decode("GB2312")}"',
    PARAM_ADDR:    lambda d, ram_size: f"{op_vaddr(d, ram_size):#x}",
    PARAM_ADDR_U8: lambda d, ram_size: f"{op_vaddr(d, ram_size):#x}, {d[op_vaddr_size(ram_size)]}",
    PARAM_JMP:     lambda d, ram_size: f"{d[0] + (d[1] << 8) + (d[2] << 16):#x}",
    PARAM_PRESET:  lambda d, ram_size: f"{op_vaddr(d, ram_size):#x}, {d[op_vaddr_size(ram_size)+2:]}"
}
