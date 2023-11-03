#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <queue>

#include "lava.h"
#include "lava_proc.h"

#define DEBUG_PRINT 0

#define TODO()      throw std::runtime_error(std::string("PROC_TODO:") + std::to_string(__LINE__) + " " + __FUNCSIG__)
#define PROC_TODO() std::cerr << "PROC_TODO: " << __FUNCSIG__ << std::endl

#if DEBUG_PRINT
#define DEBUG_WRAP_PRINT(v) std::cerr << v
#else
#define DEBUG_WRAP_PRINT(...)
#endif



void LavaProc::load(const std::vector<uint8_t> &source, uint32_t rambits, bool pen_input)
{
    this->source = source;
    this->rambits = rambits;
    this->pen_input = pen_input;

    ram.init(rambits);
    pc = 16;

    uint32_t ofs = pc;
    while (ofs < source.size()) {
        uint32_t s = parse(ofs);
        ofs += s;
    }

#if DEBUG_PRINT
    std::cerr << "OP code:" << std::endl;

    for (auto const &op: op_exec) {
        std::cerr << std::showbase << std::internal << std::setfill('0');
        std::cerr << std::hex << std::setw(10) << op.first << ": ";
        std::cerr << op_info[source[op.first]].name; // << std::endl;

#if 0
        std::stringstream dss;
        for (auto const &v: op.second.data) {
            dss << ", 0x";
            dss << std::hex << std::setfill('0') << std::setw(2) << (uint32_t)v;
        }
        std::cerr << dss.str()
#endif
        std::cerr << std::endl;
    }
#endif
}

uint32_t LavaProc::parse(uint32_t ofs)
{
    uint8_t opcode = source[ofs];
    if (op_info.find(opcode) == op_info.cend())
        throw std::runtime_error("Unknown opcode: " + std::to_string((uint32_t)opcode));
    op_info_t info = op_info[opcode];

    uint32_t size = info.len & OpParamBaseMask;
    if (info.len & OpParamAddr)
        size += rambits <= 16 ? 2 : 3;
    if (info.len & OpParamStr) {
        uint32_t i = ofs + 1;
        while (source.at(i) != '\0')
            i++;
        size += i - ofs;
    }
    if (info.len & OpParamPreset) {
        uint16_t len = source.at(ofs + size - 2) | (source.at(ofs + size - 1) << 8);
        size += len;
    }
    op_exec[ofs] = op_t{
        std::bind(info.func, this,
                  std::vector<uint8_t>(source.begin() + ofs + 1, source.begin() + ofs + size)),
    };
    return size;
}

void LavaProc::reset()
{
    ram.init(rambits);
    pc = 16;
    cb_func.func = nullptr;
}

void LavaProc::run()
{
    if (cb_func.func) {
        int32_t ret = cb_func.func();
        if (ret < 0)
            return;
        if (cb_func.stack) {
            ram.pop();
            ram.push(ret);
        }
        cb_func.func = nullptr;
    }

    auto const &op = op_exec[pc];

#if DEBUG_PRINT
    static bool print = true;
    if (print) {
        std::cerr << "OP execution:" << std::endl;
        print = false;
    }

    std::cerr << std::noshowbase << std::internal << std::setfill('0');
    std::cerr << std::hex << std::setw(8) << pc << " ";
    std::cerr << std::setw(4) << ram.getStack() << "-";
    std::cerr << std::setw(4) << ram.getLocalStack() << "-";
    std::cerr << std::setw(4) << ram.getLocalStackBp() << ": ";
    std::cerr << std::showbase;

#endif

    op.func();

    if (disp->refreshRequest())
        cb->refresh(disp->getFramebuffer());
}

#include "lava_op_code.h"
#include "lava_proc_printf.h"

uint32_t LavaProc::lava_op_push_u8(uint8_t dp0)
{
    return (uint32_t)dp0;
}

uint32_t LavaProc::lava_op_push_i16(int16_t dp0)
{
    return (uint32_t)(int32_t)dp0;
}

uint32_t LavaProc::lava_op_push_i32(int32_t dp0)
{
    return (uint32_t)dp0;
}

uint32_t LavaProc::lava_op_pushv_u8(uint32_t dp0)
{
    return (uint32_t)ram.readU8(dp0);
}

uint32_t LavaProc::lava_op_pushv_i16(uint32_t dp0)
{
    return (uint32_t)(int32_t)ram.readI16(dp0);
}

uint32_t LavaProc::lava_op_pushv_i32(uint32_t dp0)
{
    return (uint32_t)ram.readI32(dp0);
}

uint32_t LavaProc::lava_op_pushg_u8(uint32_t dp0, uint32_t ds0)
{
    return (uint32_t)ram.readU8(dp0 + ds0);
}

uint32_t LavaProc::lava_op_pushg_i16(uint32_t dp0, uint32_t ds0)
{
    return (uint32_t)(int32_t)ram.readI16(dp0 + ds0);
}

uint32_t LavaProc::lava_op_pushg_i32(uint32_t dp0, uint32_t ds0)
{
    return (uint32_t)ram.readI32(dp0 + ds0);
}

uint32_t LavaProc::lava_op_pusha_u8(uint32_t dp0, uint32_t ds0)
{
    uint32_t a = dp0 + ds0;
    return ram.getAddrVariant(a, 1);
}

uint32_t LavaProc::lava_op_pusha_i16(uint32_t dp0, uint32_t ds0)
{
    uint32_t a = dp0 + ds0;
    return ram.getAddrVariant(a, 2);
}

uint32_t LavaProc::lava_op_pusha_i32(uint32_t dp0, uint32_t ds0)
{
    uint32_t a = dp0 + ds0;
    return ram.getAddrVariant(a, 0);
}

uint32_t LavaProc::lava_op_push_str(const std::vector<uint8_t> &dp0)
{
    return ram.pushString(dp0);
}

uint32_t LavaProc::lava_op_pushlv_u8(uint32_t dp0)
{
    uint32_t addr = dp0 + ram.getLocalStackBp();
    return ram.readU8(addr);
}

uint32_t LavaProc::lava_op_pushlv_i16(uint32_t dp0)
{
    uint32_t addr = dp0 + ram.getLocalStackBp();
    return ram.readI16(addr);
}

uint32_t LavaProc::lava_op_pushlv_i32(uint32_t dp0)
{
    uint32_t addr = dp0 + ram.getLocalStackBp();
    return ram.readI32(addr);
}

uint32_t LavaProc::lava_op_pushlg_char(uint32_t dp0, uint32_t ds0)
{
    uint32_t addr = dp0 + ds0 + ram.getLocalStackBp();
    return ram.readU8(addr);
}

uint32_t LavaProc::lava_op_pushla_u8(uint32_t dp0, uint32_t ds0)
{
    uint32_t a = dp0 + ds0 + ram.getLocalStackBp();
    return ram.getAddrVariant(a, 1);
}

uint32_t LavaProc::lava_op_pushla_i32(uint32_t dp0, uint32_t ds0)
{
    uint32_t a = dp0 + ds0 + ram.getLocalStackBp();
    return ram.getAddrVariant(a, 2);
}

uint32_t LavaProc::lava_op_pushl_i32(uint32_t dp0)
{
    uint32_t a = dp0 + ram.getLocalStackBp();
    return a & ram.getAddrMask();
}

uint32_t LavaProc::lava_op_pre_inc(uint32_t ds0)
{
    int32_t addr = ds0;
    int32_t v = ram.readVariant(addr) + 1;
    ram.writeVariant(addr, v);
    return v;
}

uint32_t LavaProc::lava_op_pre_dec(uint32_t ds0)
{
    int32_t addr = ds0;
    int32_t v = ram.readVariant(addr) - 1;
    ram.writeVariant(addr, v);
    return v;
}

uint32_t LavaProc::lava_op_post_inc(uint32_t ds0)
{
    int32_t addr = ds0;
    int32_t v = ram.readVariant(addr);
    ram.writeVariant(addr, v + 1);
    return v;
}

uint32_t LavaProc::lava_op_post_dec(uint32_t ds0)
{
    int32_t addr = ds0;
    int32_t v = ram.readVariant(addr);
    ram.writeVariant(addr, v - 1);
    return v;
}

uint32_t LavaProc::lava_op_add(uint32_t ds0, uint32_t ds1)
{
    return (uint32_t)((int32_t)ds1 + (int32_t)ds0);
}

uint32_t LavaProc::lava_op_sub(uint32_t ds0, uint32_t ds1)
{
    return (uint32_t)((int32_t)ds1 - (int32_t)ds0);
}

uint32_t LavaProc::lava_op_and(uint32_t ds0, uint32_t ds1)
{
    return ds1 & ds0;
}

uint32_t LavaProc::lava_op_or(uint32_t ds0, uint32_t ds1)
{
    return ds1 | ds0;
}

uint32_t LavaProc::lava_op_xor(uint32_t ds0, uint32_t ds1)
{
    return ds1 ^ ds0;
}

uint32_t LavaProc::lava_op_land(uint32_t ds0, uint32_t ds1)
{
    return (ds0 && ds1) ? Lava::True : Lava::False;
}

uint32_t LavaProc::lava_op_lor(uint32_t ds0, uint32_t ds1)
{
    return (ds0 || ds1) ? Lava::True : Lava::False;
}

uint32_t LavaProc::lava_op_mul(uint32_t ds0, uint32_t ds1)
{
    return (uint32_t)((int32_t)ds1 * (int32_t)ds0);
}

uint32_t LavaProc::lava_op_div(uint32_t ds0, uint32_t ds1)
{
    return (uint32_t)((int32_t)ds1 / (int32_t)ds0);
}

uint32_t LavaProc::lava_op_mod(uint32_t ds0, uint32_t ds1)
{
    TODO();
    return 0;
}

uint32_t LavaProc::lava_op_equ(uint32_t ds0, uint32_t ds1)
{
    return ds1 == ds0 ? Lava::True : Lava::False;
}

uint32_t LavaProc::lava_op_neq(uint32_t ds0, uint32_t ds1)
{
    return ds1 != ds0 ? Lava::True : Lava::False;
}

uint32_t LavaProc::lava_op_le(uint32_t ds0, uint32_t ds1)
{
    return ((int32_t)ds1 <= (int32_t)ds0) ? Lava::True : Lava::False;
}

uint32_t LavaProc::lava_op_ge(uint32_t ds0, uint32_t ds1)
{
    return ((int32_t)ds1 >= (int32_t)ds0) ? Lava::True : Lava::False;
}

uint32_t LavaProc::lava_op_great(uint32_t ds0, uint32_t ds1)
{
    return ((int32_t)ds1 > (int32_t)ds0) ? Lava::True : Lava::False;
}

uint32_t LavaProc::lava_op_less(uint32_t ds0, uint32_t ds1)
{
    return ((int32_t)ds1 < (int32_t)ds0) ? Lava::True : Lava::False;
}

uint32_t LavaProc::lava_op_let(uint32_t ds0, uint32_t ds1)
{
    int32_t v = ds0;
    int32_t a = ds1;
    ram.writeVariant(a, v);
    return v;
}

uint32_t LavaProc::lava_op_ptr(uint32_t ds0)
{
    return ram.readU8(ds0);
}

void LavaProc::lava_op_pop(uint32_t ds0)
{
    flagv = ds0;
}

bool LavaProc::lava_op_jmpe(uint32_t dp0)
{
    if (!flagv)
        pc = dp0;
    return !flagv;
}

bool LavaProc::lava_op_jmpn(uint32_t dp0)
{
    if (flagv)
        pc = dp0;
    return flagv;
}

bool LavaProc::lava_op_jmp(uint32_t dp0)
{
    pc = dp0;
    return true;
}

void LavaProc::lava_op_set_sp(uint32_t dp0)
{
    ram.setLocalStack(dp0);
}

bool LavaProc::lava_op_call(uint32_t dp0)
{
    ram.pushInst(pc + 3);   // Address of the next instruction
    return lava_op_jmp(dp0);
}

void LavaProc::lava_op_add_bp(uint32_t dp0, uint8_t dp1)
{
    std::cerr << "PROC_TODO: " << __FUNCSIG__ << std::endl;

    int32_t t;
    uint8_t t2;
    int32_t i;
    uint32_t local_bp = ram.getLocalStackBp();
    uint32_t local_sp = ram.getLocalStack();
    uint32_t eval_top = ram.getStack();

    t=local_bp;
    local_bp=local_sp;

    ram.writeU8(local_bp+3, t & 0xff);
    ram.writeU8(local_bp+4, (t>>8) & 0xff);
    if (rambits > 16)
        ram.writeU8(local_bp+5, (t>>16) & 0xff);

    t = dp0;
    local_sp=local_bp+(t&0xffffff);
    //if (local_sp&3) local_sp+=4-(local_sp&3); //令堆栈开始于4字节边界

    t = dp1 * 4;
    if (t) {
        eval_top -= t;
        t2 = eval_top;
        i=0;
        if (rambits == 32) {
            while (t) {
                uint8_t v = ram.readU8(LAVA_STACK_OFFSET + t2++);
                ram.writeU8(local_bp+8+i++, v);
                t--;
            }
        } else if (rambits > 16) {
            while (t) {
                uint8_t v = ram.readU8(LAVA_STACK_OFFSET + t2++);
                ram.writeU8(local_bp+6+i++, v);
                t--;
            }
        } else {
            while (t) {
                uint8_t v = ram.readU8(LAVA_STACK_OFFSET + t2++);
                ram.writeU8(local_bp+5+i++, v);
                t--;
            }
        }
    }

    ram.setLocalStackBp(local_bp);
    ram.setLocalStack(local_sp);
    ram.setStack(eval_top);
}

void LavaProc::lava_op_sub_bp()
{
    ram.setLocalStack(ram.getLocalStackBp());

    uint32_t t = 0;
    t |= ram.readU8(ram.getLocalStackBp() + 0) <<  0;
    t |= ram.readU8(ram.getLocalStackBp() + 1) <<  8;
    t |= ram.readU8(ram.getLocalStackBp() + 2) << 16;

    pc = t;

    t = 0;
    t |= ram.readU8(ram.getLocalStackBp() + 3) <<  0;
    t |= ram.readU8(ram.getLocalStackBp() + 4) <<  8;
    t |= ram.readU8(ram.getLocalStackBp() + 5) << 16;
    t &= ram.getAddrMask();

    ram.setLocalStackBp(t);

    PROC_TODO();
    // if (func_top) cur_funcid=func_stack[--func_top];
}

void LavaProc::lava_op_quit()
{
    lava_op_exit(0);
}

void LavaProc::lava_op_preset(uint32_t dp0, const std::vector<uint8_t> &dp2)
{
    ram.writeData(dp0, dp2);
}

uint32_t LavaProc::lava_op_qadd(int16_t dp0, uint32_t ds0)
{
    return (int32_t)ds0 + (int32_t)dp0;
}

uint32_t LavaProc::lava_op_qsub(int16_t dp0, uint32_t ds0)
{
    return (int32_t)ds0 - (int32_t)dp0;
}

uint32_t LavaProc::lava_op_qmul(int16_t dp0, uint32_t ds0)
{
    return (int32_t)ds0 * (int32_t)dp0;
}

uint32_t LavaProc::lava_op_qdiv(int16_t dp0, uint32_t ds0)
{
    return (int32_t)ds0 / (int32_t)dp0;
}

uint32_t LavaProc::lava_op_qmod(int16_t dp0, uint32_t ds0)
{
    return (int32_t)ds0 % dp0;
}

uint32_t LavaProc::lava_op_qlshift(int16_t dp0, uint32_t ds0)
{
    uint32_t v = ds0;
    int32_t shift = dp0;
    if (shift >= 0)
        v <<= shift;
    else
        v >>= -shift;
    return v;
}

uint32_t LavaProc::lava_op_qrshift(int16_t dp0, uint32_t ds0)
{
    uint32_t v = ds0;
    int32_t shift = dp0;
    if (shift >= 0)
        v >>= shift;
    else
        v <<= -shift;
    return v;
}

uint32_t LavaProc::lava_op_qequ(int16_t dp0, uint32_t ds0)
{
    return (int32_t)ds0 == (int32_t)dp0 ? Lava::True : Lava::False;
}

uint32_t LavaProc::lava_op_qneq(int16_t dp0, uint32_t ds0)
{
    return (int32_t)ds0 != (int32_t)dp0 ? Lava::True : Lava::False;
}

uint32_t LavaProc::lava_op_qgreat(int16_t dp0, uint32_t ds0)
{
    return (int32_t)ds0 > (int32_t)dp0 ? Lava::True : Lava::False;
}

uint32_t LavaProc::lava_op_qless(int16_t dp0, uint32_t ds0)
{
    return (int32_t)ds0 < (int32_t)dp0 ? Lava::True : Lava::False;
}

uint32_t LavaProc::lava_op_qge(int16_t dp0, uint32_t ds0)
{
    return (int32_t)ds0 >= (int32_t)dp0 ? Lava::True : Lava::False;
}

uint32_t LavaProc::lava_op_qle(int16_t dp0, uint32_t ds0)
{
    return (int32_t)ds0 <= (int32_t)dp0 ? Lava::True : Lava::False;
}

uint32_t LavaProc::lava_op_getchar()
{
    cb_func.func = std::bind(&LavaCallback::getchar, cb);
    cb_func.stack = true;
    return 0;
}

void LavaProc::lava_op_printf(uint32_t ds0)
{
    TODO();
}

void LavaProc::lava_op_strcpy(uint32_t ds0, uint32_t ds1)
{
    TODO();
}

uint32_t LavaProc::lava_op_strlen(uint32_t ds0)
{
    uint32_t addr = ds0;
    return ram.strlen(addr);
}

void LavaProc::lava_op_setscreen(uint32_t ds0)
{
    uint8_t mode = ds0;
    PROC_TODO();
    return;
#if 0
    if (mode) {
        scr_mode=1;
        curr_CPR=((ScreenWidth-2)/6)&0xfe;
        curr_RPS=(ScreenHeight-1)/13;
        small_left=(ScreenWidth-curr_CPR*6)/2;
        small_up=(ScreenHeight-(curr_RPS*13-1))/2;
        small_down=ScreenHeight-(curr_RPS*13-1)-small_up;
    } else {
        scr_mode=0;
        curr_CPR=ScreenWidth/8;
        curr_RPS=ScreenHeight/16;
    }
    scr_x=0;scr_y=0;scr_off=0;
    memset(lRam+TextBuffer,0,curr_CPR*curr_RPS);
#endif
    TODO();
}

void LavaProc::lava_op_delay(uint32_t ds0)
{
    cb_func.func = std::bind(&LavaCallback::delay_ms, cb, ds0);
    cb_func.stack = false;
}

void LavaProc::lava_op_writeblock(uint32_t ds0, uint32_t ds1, uint32_t ds2, uint32_t ds3, uint32_t ds4, uint32_t ds5)
{
    uint32_t addr = ds0;
    uint8_t cfg = ds1;
    uint16_t h = ds2;
    uint16_t w = ds3;
    uint16_t y = ds4;
    uint16_t x = ds5;

    uint32_t size = 0;
    switch (disp->getMode()) {
    case LavaDisp::GraphicMono:
        size = ((w + 7) / 8) * h;
        break;
    case LavaDisp::Graphic16:
        size = ((w + 1) / 2) * h;
        break;
    case LavaDisp::Graphic256:
        size = w * h;
        break;
    }

    auto const &data = ram.readData(addr, size);

    disp->drawBlock(x, y, w, h, cfg, data);
    //std::cerr << __func__ << ": " << (uint32_t)cfg << " - " << w << " x " << h << std::endl;
}

void LavaProc::lava_op_fbswap()
{
    disp->framebufferSwap();
}

void LavaProc::lava_op_textout(uint32_t ds0, uint32_t ds1, uint32_t ds2, uint32_t ds3)
{
    uint8_t cfg = ds0;
    uint32_t addr = ds1;
    uint16_t x = ds3;
    uint16_t y = ds2;
    auto const &str = ram.readStringData(addr);
    //std::cerr << __func__ << ": " << addr << ", " << str.size() << std::endl;
    disp->drawText(str, x, y, cfg);
}

void LavaProc::lava_op_block(uint32_t ds0, uint32_t ds1, uint32_t ds2, uint32_t ds3, uint32_t ds4)
{
    uint8_t cfg = ds0;
    uint16_t y1 = ds1;
    uint16_t x1 = ds2;
    uint16_t y0 = ds3;
    uint16_t x0 = ds4;
    disp->drawBlock(x0, x1, y0, y1, cfg);
}

void LavaProc::lava_op_rectangle(uint32_t ds0, uint32_t ds1, uint32_t ds2, uint32_t ds3, uint32_t ds4)
{
    uint8_t cfg = ds0;
    uint16_t y1 = ds1;
    uint16_t x1 = ds2;
    uint16_t y0 = ds3;
    uint16_t x0 = ds4;
    disp->drawRectangle(x0, x1, y0, y1, cfg);
}

void LavaProc::lava_op_exit(uint32_t ds0)
{
    cb->exit(ds0);
}

void LavaProc::lava_op_clearscreen()
{
    disp->clearScreen();
}

uint32_t LavaProc::lava_op_rand()
{
    int32_t v = seed * 0x15a4e35 + 1;
    seed = v;
    return (v >> 16) & 0x7fff;
}

void LavaProc::lava_op_srand(uint32_t ds0)
{
    seed = ds0;
}

void LavaProc::lava_op_locate(uint32_t ds0, uint32_t ds1)
{
    TODO();
}

uint32_t LavaProc::lava_op_inkey()
{
    return cb->in_key();
}

void LavaProc::lava_op_point(uint32_t ds0, uint32_t ds1, uint32_t ds2)
{
    TODO();
}

void LavaProc::lava_op_line(uint32_t ds0, uint32_t ds1, uint32_t ds2, uint32_t ds3, uint32_t ds4)
{
    TODO();
}

void LavaProc::lava_op_circle(uint32_t ds0, uint32_t ds1, uint32_t ds2, uint32_t ds3, uint32_t ds4)
{
    TODO();
}

uint32_t LavaProc::lava_op_strchr(uint32_t ds0, uint32_t ds1)
{
    const uint8_t *pstr = ram.data().data() + (ds1 & ram.getAddrMask());
    uint8_t c = ds0;

    const uint8_t *s = pstr;
    while (*s != 0 && *s != c)
        s++;
    if (*s == c)
        return s - pstr;
    // Note if c appears at index 0, this cannot be distinguished
    return 0;
}

uint32_t LavaProc::lava_op_strcmp(uint32_t ds0, uint32_t ds1)
{
    const char *s1 = reinterpret_cast<const char *>(ram.data().data()) + (ds1 & ram.getAddrMask());
    const char *s2 = reinterpret_cast<const char *>(ram.data().data()) + (ds0 & ram.getAddrMask());
    for (; *s1 == *s2; s1++, s2++)
        if (*s1 == 0)
            return 0;
    if (*s1 < *s2)
        return -1;
    return 1;
}

uint32_t LavaProc::lava_op_strstr(uint32_t ds0, uint32_t ds1)
{
    TODO();
    return 0;
}

void LavaProc::lava_op_memset(uint32_t ds0, uint32_t ds1, uint32_t ds2)
{
    TODO();
}

void LavaProc::lava_op_memcpy(uint32_t ds0, uint32_t ds1, uint32_t ds2)
{
    int32_t len = ds0 & ram.getAddrMask();
    int32_t src = ds1 & ram.getAddrMask();
    int32_t dst = ds2 & ram.getAddrMask();
    std::copy(ram.data().begin() + src, ram.data().begin() + src + len,
              ram.data().begin() + dst);
}

uint32_t LavaProc::lava_op_fopen(uint32_t ds0, uint32_t ds1)
{
    auto const &path = to_string(ram.readStringData(ds1));
    auto const &mode = to_string(ram.readStringData(ds0));
    return cb->fopen(path, mode);
}

void LavaProc::lava_op_fclose(uint32_t ds0)
{
    cb->fclose(ds0);
}

uint32_t LavaProc::lava_op_fread(uint32_t ds0, uint32_t ds1, uint32_t ds2, uint32_t ds3)
{
    uint8_t fd = ds0;
    uint32_t size = ds1 * ds2;
    uint32_t addr = ds3;

    auto const &data = cb->fread(fd, size);
    ram.writeData(addr, data);
    return data.size();
}

uint32_t LavaProc::lava_op_fwrite(uint32_t ds0, uint32_t ds1, uint32_t ds2, uint32_t ds3)
{
    uint8_t fd = ds0;
    uint32_t size = ds1 * ds2;
    uint32_t addr = ds3;

    auto const &data = ram.readData(addr, size);
    return cb->fwrite(fd, data);
}

uint32_t LavaProc::lava_op_fseek(uint32_t ds0, uint32_t ds1, uint32_t ds2)
{
    LavaCallback::fseek_mode_t mode = (LavaCallback::fseek_mode_t)ds0;
    int32_t ofs = ds1;
    uint8_t fd = ds2;
    return cb->fseek(fd, ofs, mode);
}

void LavaProc::lava_op_rewind(uint32_t ds0)
{
    uint8_t fd = ds0;
    cb->fseek(fd, 0, LavaCallback::SeekSet);
}

uint32_t LavaProc::lava_op_gettick()
{
    int32_t ms = cb->get_ms();
    int32_t tick = (ms % 1000) * 256 / 1000;
    //std::cerr << __func__ << ": " << ms << ", " << tick << std::endl;
    return tick;
}

uint32_t LavaProc::lava_op_checkkey(uint32_t ds0)
{
    return cb->check_key(ds0);
}

void LavaProc::lava_op_releasekey(uint32_t ds0)
{
    TODO();
}

void LavaProc::lava_op_getblock(uint32_t ds0, uint32_t ds1, uint32_t ds2, uint32_t ds3, uint32_t ds4, uint32_t ds5)
{
    int32_t dst = ds0;
    int32_t cfg = ds1;
    uint16_t h = ds2;
    uint16_t w = ds3;
    uint16_t y = ds4;
    uint16_t x = ds5;

    bool no_buf = cfg & 0x40;

    ram.writeData(dst, disp->getBlock(!no_buf, x, y, w, h));
}

uint32_t LavaProc::lava_op_setgraphmode(uint32_t ds0)
{
    LavaDisp::mode_t mode = disp->getMode();
    disp->setMode((LavaDisp::mode_t)ds0);
    return (uint32_t)mode;
}

void LavaProc::lava_op_setbgcolor(uint32_t ds0)
{
    disp->setBackgroundColour(ds0);
}

void LavaProc::lava_op_setfgcolor(uint32_t ds0)
{
    disp->setForegroundColour(ds0);
}
