#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstring>
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
    pc = 16;    // Skip file header

    // Parse and cache all instructions
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
        std::cerr << op_info[source[op.first]].name << std::endl;
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
    std::cerr << std::setw(4) << ram.getFuncStackStart() << "-";
    std::cerr << std::setw(4) << ram.getFuncStackEnd() << ": ";
    std::cerr << std::showbase;

#endif

    cb_func.pc = pc;
    cb_func.sp = ram.getStack();
    op.func();

    if (disp->refreshRequest())
        cb->refresh(disp->getFramebuffer());
}

void LavaProc::saveState(std::vector<uint8_t> &data)
{
    auto const pushU32 = [&](uint32_t v)
    {
        data.push_back(v >>  0);
        data.push_back(v >>  8);
        data.push_back(v >> 16);
        data.push_back(v >> 24);
    };

    // Common configurations
    pushU32(rambits);
    pushU32(pen_input);

    // System state
    if (cb_func.func) {
        // Save state before waiting for callback
        pushU32(cb_func.pc);
        pushU32(cb_func.sp);
    } else {
        // Current state is ready for save state
        pushU32(pc);
        pushU32(ram.getStack());
    }
    // Instructions requiring callback do no affect these, so should be safe
    pushU32(ram.getFuncStackStart());
    pushU32(ram.getFuncStackEnd());
    pushU32(ram.getStringStack());
    pushU32(flagv);
    pushU32(seed);

    data.insert(data.end(), ram.data().cbegin(), ram.data().cend());
}

uint32_t LavaProc::restoreState(const std::vector<uint8_t> &data, uint32_t offset)
{
    auto const popU32 = [&]() -> uint32_t
    {
        uint32_t v = 0;
        v |= data[offset + 0] <<  0;
        v |= data[offset + 1] <<  8;
        v |= data[offset + 2] << 16;
        v |= data[offset + 3] << 24;
        offset += 4;
        return v;
    };

    rambits = popU32();
    pen_input = popU32();
    ram.init(rambits);

    cb_func.func = nullptr;
    pc = popU32();
    ram.setStack(popU32());
    ram.setFuncStackStart(popU32());
    ram.setFuncStackEnd(popU32());
    ram.setStringStack(popU32());
    flagv = popU32();
    seed = popU32();

    std::copy(data.begin() + offset, data.begin() + offset + ram.data().size(), ram.data().begin());
    offset += ram.data().size();

    return offset;
}

#include "lava_op_code.h"
#include "lava_proc_printf.h"

uint32_t LavaProc::lava_op_push_u8(uint8_t dp0)
{
    return dp0;
}

uint32_t LavaProc::lava_op_push_i16(int16_t dp0)
{
    return (int32_t)dp0;
}

uint32_t LavaProc::lava_op_push_i32(int32_t dp0)
{
    return dp0;
}

uint32_t LavaProc::lava_op_pushv_u8(uint32_t dp0)
{
    return ram.readU8(dp0);
}

uint32_t LavaProc::lava_op_pushv_i16(uint32_t dp0)
{
    return (int32_t)ram.readI16(dp0);
}

uint32_t LavaProc::lava_op_pushv_i32(uint32_t dp0)
{
    return ram.readI32(dp0);
}

uint32_t LavaProc::lava_op_pushg_u8(uint32_t dp0, uint32_t ds0)
{
    return ram.readU8(dp0 + ds0);
}

uint32_t LavaProc::lava_op_pushg_i16(uint32_t dp0, uint32_t ds0)
{
    return (int32_t)ram.readI16(dp0 + ds0);
}

uint32_t LavaProc::lava_op_pushg_i32(uint32_t dp0, uint32_t ds0)
{
    return ram.readI32(dp0 + ds0);
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
    uint32_t addr = dp0 + ram.getFuncStackStart();
    return ram.readU8(addr);
}

uint32_t LavaProc::lava_op_pushlv_i16(uint32_t dp0)
{
    uint32_t addr = dp0 + ram.getFuncStackStart();
    return (int32_t)ram.readI16(addr);
}

uint32_t LavaProc::lava_op_pushlv_i32(uint32_t dp0)
{
    uint32_t addr = dp0 + ram.getFuncStackStart();
    return ram.readI32(addr);
}

uint32_t LavaProc::lava_op_pushlg_char(uint32_t dp0, uint32_t ds0)
{
    uint32_t addr = dp0 + ds0 + ram.getFuncStackStart();
    return ram.readU8(addr);
}

uint32_t LavaProc::lava_op_pushlg_int(uint32_t dp0, uint32_t ds0)
{
    uint32_t addr = dp0 + ds0 + ram.getFuncStackStart();
    return (int32_t)ram.readI16(addr);
}

uint32_t LavaProc::lava_op_pushlg_long(uint32_t dp0, uint32_t ds0)
{
    uint32_t addr = dp0 + ds0 + ram.getFuncStackStart();
    return ram.readI32(addr);
}

uint32_t LavaProc::lava_op_pushla_u8(uint32_t dp0, uint32_t ds0)
{
    uint32_t a = dp0 + ds0 + ram.getFuncStackStart();
    return ram.getAddrVariant(a, 1);
}

uint32_t LavaProc::lava_op_pushla_i32(uint32_t dp0, uint32_t ds0)
{
    uint32_t a = dp0 + ds0 + ram.getFuncStackStart();
    return ram.getAddrVariant(a, 2);
}

uint32_t LavaProc::lava_op_pushl_i32(uint32_t dp0)
{
    uint32_t a = dp0 + ram.getFuncStackStart();
    return a & ram.getAddrMask();
}

uint32_t LavaProc::lava_op_neg(uint32_t ds0)
{
    return -(int32_t)ds0;
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

uint32_t LavaProc::lava_op_lnot(uint32_t ds0)
{
    return ds0 ? Lava::False : Lava::True;
}

uint32_t LavaProc::lava_op_mul(uint32_t ds0, uint32_t ds1)
{
    return (int32_t)ds1 * (int32_t)ds0;
}

uint32_t LavaProc::lava_op_div(uint32_t ds0, uint32_t ds1)
{
    return (int32_t)ds1 / (int32_t)ds0;
}

uint32_t LavaProc::lava_op_mod(uint32_t ds0, uint32_t ds1)
{
    int32_t a3 = ds0;
    int32_t a1 = ds1;
    return a1 % a3;
}

uint32_t LavaProc::lava_op_lshift(uint32_t ds0, uint32_t ds1)
{
    int32_t a3 = ds0;
    int32_t a1 = ds1;

    if (a3 < 0)
        return 0;
    return a1 << a3;
}

uint32_t LavaProc::lava_op_rshift(uint32_t ds0, uint32_t ds1)
{
    int32_t a3 = ds0;
    int32_t a1 = ds1;

    if (a3 < 0)
        return 0;
    return (uint32_t)a1 >> a3;
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

uint32_t LavaProc::lava_op_cptr(uint32_t ds0)
{
    return (ds0 & ram.getAddrMask()) | (0x01 << (8 * ram.getAddrBytes()));
}

void LavaProc::lava_op_pop(uint32_t ds0)
{
    flagv = ds0;
}

void LavaProc::lava_op_jmpe(uint32_t dp0)
{
    if (!flagv)
        pc = dp0;
}

void LavaProc::lava_op_jmpn(uint32_t dp0)
{
    if (flagv)
        pc = dp0;
}

void LavaProc::lava_op_jmp(uint32_t dp0)
{
    pc = dp0;
}

void LavaProc::lava_op_set_sp(uint32_t dp0)
{
    ram.setFuncStackEnd(dp0);
}

void LavaProc::lava_op_call(uint32_t dp0)
{
    // Function call, first step, PC jump

    // Write PC address of next instruction to next stack frame
    ram.writeU24(ram.getFuncStackEnd(), pc);
    lava_op_jmp(dp0);
}

void LavaProc::lava_op_push_frame(uint32_t dp0, uint8_t dp1)
{
    // Function call, second step, update stack frame

    // PC address already written by previous call(), skip
    uint32_t ofs = ram.getFuncStackEnd() + 3;

    // Write start of previous stack frame
    ram.writeAddr(ofs, ram.getFuncStackStart());
    ofs += ram.getAddrBytes();

    // Special offset for 32-bit mode?
    if (rambits >= 32)
        ofs += 1;

    // Write stack frame variables
    uint32_t nwords = dp1;
    while (nwords--) {
        uint32_t v = ram.pop();
        ram.writeU32(ofs + nwords * 4, v);
    }

    // Update stack frame start & end pointers to current frame
    uint32_t frame_size = dp0;
    ram.setFuncStackStart(ram.getFuncStackEnd());
    ram.setFuncStackEnd(ram.getFuncStackEnd() + frame_size);
}

void LavaProc::lava_op_pop_frame()
{
    // Function call return

    // Read current stack frame
    uint32_t sp = ram.getFuncStackStart();

    // Read and jump to return PC address
    pc = ram.readU24(sp);

    // Read previous stack frame start
    sp = ram.readAddr(sp + 3);

    // Update stack frame start & end pointers to previous frame
    ram.setFuncStackEnd(ram.getFuncStackStart());
    ram.setFuncStackStart(sp);
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
    cb_func.func = std::bind(&LavaCallback::delayMs, cb, ds0);
    cb_func.stack = false;
}

void LavaProc::lava_op_writeblock(uint32_t ds0, uint32_t ds1, uint32_t ds2, uint32_t ds3, uint32_t ds4, uint32_t ds5)
{
    uint32_t addr = ds0;
    uint8_t cfg = ds1;
    uint16_t h = ds2;
    uint16_t w = ds3;
    int16_t y = ds4;
    int16_t x = ds5;

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
}

void LavaProc::lava_op_fbflush()
{
    disp->framebufferFlush();
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
    int16_t y1 = ds1;
    int16_t x1 = ds2;
    int16_t y0 = ds3;
    int16_t x0 = ds4;
    disp->drawBlock(x0, x1, y0, y1, cfg);
}

void LavaProc::lava_op_rectangle(uint32_t ds0, uint32_t ds1, uint32_t ds2, uint32_t ds3, uint32_t ds4)
{
    uint8_t cfg = ds0;
    int16_t y1 = ds1;
    int16_t x1 = ds2;
    int16_t y0 = ds3;
    int16_t x0 = ds4;
    disp->drawRectangle(x0, x1, y0, y1, cfg);
}

void LavaProc::lava_op_exit(uint32_t ds0)
{
    cb->exit(ds0);
}

void LavaProc::lava_op_clearscreen()
{
    disp->clearWorking();
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
    uint8_t x = ds0;
    uint8_t y = ds1;

#if 0
    uint8_t t;
    t = x;

    if (t>=curr_CPR) t=curr_CPR-1;
    scr_x=t;

    t = y;
    if (t>=curr_RPS) t=curr_RPS-1;
    scr_y=t;

    scr_off=scr_y*curr_CPR+scr_x;
#endif
    TODO();
}

uint32_t LavaProc::lava_op_inkey()
{
    return cb->inKey();
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

uint32_t LavaProc::lava_op_isalpha(uint32_t ds0)
{
    char c = ds0;
    if (c >= 'a' && c <= 'z')
        return Lava::True;
    if (c >= 'A' && c <= 'Z')
        return Lava::True;
    return Lava::False;
}

void LavaProc::lava_op_strcat(uint32_t ds0, uint32_t ds1)
{
    uint8_t *dst = ram.data().data() + (ds1 & ram.getAddrMask());
    uint8_t *src = ram.data().data() + (ds0 & ram.getAddrMask());

    while (*dst != 0)
        dst++;

    do
        *dst++ = *src;
    while (*src++ != 0);
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
    const char *haystack = reinterpret_cast<const char *>(ram.data().data()) + (ds1 & ram.getAddrMask());
    const char *needle   = reinterpret_cast<const char *>(ram.data().data()) + (ds0 & ram.getAddrMask());

    const char *p = strstr(haystack, needle);
    // Note if needle appears at index 0, this cannot be distinguished
    if (p == nullptr)
        return 0;
    return p - haystack;
}

void LavaProc::lava_op_strcpy(uint32_t ds0, uint32_t ds1)
{
    uint8_t *dst = ram.data().data() + (ds1 & ram.getAddrMask());
    const uint8_t *src = ram.data().data() + (ds0 & ram.getAddrMask());
    do
        *dst++ = *src;
    while (*src++ != 0);
}

uint32_t LavaProc::lava_op_strlen(uint32_t ds0)
{
    uint32_t addr = ds0;
    return ram.strlen(addr);
}

void LavaProc::lava_op_memset(uint32_t ds0, uint32_t ds1, uint32_t ds2)
{
    uint32_t len = ds0 & ram.getAddrMask();
    uint8_t v = ds1;
    uint32_t addr = ds2 & ram.getAddrMask();
    std::fill(ram.data().begin() + addr, ram.data().begin() + addr + len, v);
}

void LavaProc::lava_op_memcpy(uint32_t ds0, uint32_t ds1, uint32_t ds2)
{
    uint32_t len = ds0 & ram.getAddrMask();
    uint32_t src = ds1 & ram.getAddrMask();
    uint32_t dst = ds2 & ram.getAddrMask();
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

uint32_t LavaProc::lava_op_deletefile(uint32_t ds0)
{
    uint32_t str = ds0 & ram.getAddrMask();
    std::string fpath(reinterpret_cast<const char *>(ram.data().data()) + str);
    return cb->remove(fpath);
}

uint32_t LavaProc::lava_op_crc16(uint32_t ds0, uint32_t ds1)
{
    static uint8_t crc1[256]={
        0x0,0x21,0x42,0x63,0x84,0xa5,0xc6,0xe7,
        0x8,0x29,0x4a,0x6b,0x8c,0xad,0xce,0xef,
        0x31,0x10,0x73,0x52,0xb5,0x94,0xf7,0xd6,
        0x39,0x18,0x7b,0x5a,0xbd,0x9c,0xff,0xde,
        0x62,0x43,0x20,0x1,0xe6,0xc7,0xa4,0x85,
        0x6a,0x4b,0x28,0x9,0xee,0xcf,0xac,0x8d,
        0x53,0x72,0x11,0x30,0xd7,0xf6,0x95,0xb4,
        0x5b,0x7a,0x19,0x38,0xdf,0xfe,0x9d,0xbc,
        0xc4,0xe5,0x86,0xa7,0x40,0x61,0x2,0x23,
        0xcc,0xed,0x8e,0xaf,0x48,0x69,0xa,0x2b,
        0xf5,0xd4,0xb7,0x96,0x71,0x50,0x33,0x12,
        0xfd,0xdc,0xbf,0x9e,0x79,0x58,0x3b,0x1a,
        0xa6,0x87,0xe4,0xc5,0x22,0x3,0x60,0x41,
        0xae,0x8f,0xec,0xcd,0x2a,0xb,0x68,0x49,
        0x97,0xb6,0xd5,0xf4,0x13,0x32,0x51,0x70,
        0x9f,0xbe,0xdd,0xfc,0x1b,0x3a,0x59,0x78,
        0x88,0xa9,0xca,0xeb,0xc,0x2d,0x4e,0x6f,
        0x80,0xa1,0xc2,0xe3,0x4,0x25,0x46,0x67,
        0xb9,0x98,0xfb,0xda,0x3d,0x1c,0x7f,0x5e,
        0xb1,0x90,0xf3,0xd2,0x35,0x14,0x77,0x56,
        0xea,0xcb,0xa8,0x89,0x6e,0x4f,0x2c,0xd,
        0xe2,0xc3,0xa0,0x81,0x66,0x47,0x24,0x5,
        0xdb,0xfa,0x99,0xb8,0x5f,0x7e,0x1d,0x3c,
        0xd3,0xf2,0x91,0xb0,0x57,0x76,0x15,0x34,
        0x4c,0x6d,0xe,0x2f,0xc8,0xe9,0x8a,0xab,
        0x44,0x65,0x6,0x27,0xc0,0xe1,0x82,0xa3,
        0x7d,0x5c,0x3f,0x1e,0xf9,0xd8,0xbb,0x9a,
        0x75,0x54,0x37,0x16,0xf1,0xd0,0xb3,0x92,
        0x2e,0xf,0x6c,0x4d,0xaa,0x8b,0xe8,0xc9,
        0x26,0x7,0x64,0x45,0xa2,0x83,0xe0,0xc1,
        0x1f,0x3e,0x5d,0x7c,0x9b,0xba,0xd9,0xf8,
        0x17,0x36,0x55,0x74,0x93,0xb2,0xd1,0xf0
    };
    static uint8_t crc2[256]={
        0x0,0x10,0x20,0x30,0x40,0x50,0x60,0x70,
        0x81,0x91,0xa1,0xb1,0xc1,0xd1,0xe1,0xf1,
        0x12,0x2,0x32,0x22,0x52,0x42,0x72,0x62,
        0x93,0x83,0xb3,0xa3,0xd3,0xc3,0xf3,0xe3,
        0x24,0x34,0x4,0x14,0x64,0x74,0x44,0x54,
        0xa5,0xb5,0x85,0x95,0xe5,0xf5,0xc5,0xd5,
        0x36,0x26,0x16,0x6,0x76,0x66,0x56,0x46,
        0xb7,0xa7,0x97,0x87,0xf7,0xe7,0xd7,0xc7,
        0x48,0x58,0x68,0x78,0x8,0x18,0x28,0x38,
        0xc9,0xd9,0xe9,0xf9,0x89,0x99,0xa9,0xb9,
        0x5a,0x4a,0x7a,0x6a,0x1a,0xa,0x3a,0x2a,
        0xdb,0xcb,0xfb,0xeb,0x9b,0x8b,0xbb,0xab,
        0x6c,0x7c,0x4c,0x5c,0x2c,0x3c,0xc,0x1c,
        0xed,0xfd,0xcd,0xdd,0xad,0xbd,0x8d,0x9d,
        0x7e,0x6e,0x5e,0x4e,0x3e,0x2e,0x1e,0xe,
        0xff,0xef,0xdf,0xcf,0xbf,0xaf,0x9f,0x8f,
        0x91,0x81,0xb1,0xa1,0xd1,0xc1,0xf1,0xe1,
        0x10,0x0,0x30,0x20,0x50,0x40,0x70,0x60,
        0x83,0x93,0xa3,0xb3,0xc3,0xd3,0xe3,0xf3,
        0x2,0x12,0x22,0x32,0x42,0x52,0x62,0x72,
        0xb5,0xa5,0x95,0x85,0xf5,0xe5,0xd5,0xc5,
        0x34,0x24,0x14,0x4,0x74,0x64,0x54,0x44,
        0xa7,0xb7,0x87,0x97,0xe7,0xf7,0xc7,0xd7,
        0x26,0x36,0x6,0x16,0x66,0x76,0x46,0x56,
        0xd9,0xc9,0xf9,0xe9,0x99,0x89,0xb9,0xa9,
        0x58,0x48,0x78,0x68,0x18,0x8,0x38,0x28,
        0xcb,0xdb,0xeb,0xfb,0x8b,0x9b,0xab,0xbb,
        0x4a,0x5a,0x6a,0x7a,0xa,0x1a,0x2a,0x3a,
        0xfd,0xed,0xdd,0xcd,0xbd,0xad,0x9d,0x8d,
        0x7c,0x6c,0x5c,0x4c,0x3c,0x2c,0x1c,0xc,
        0xef,0xff,0xcf,0xdf,0xaf,0xbf,0x8f,0x9f,
        0x6e,0x7e,0x4e,0x5e,0x2e,0x3e,0xe,0x1e
    };

    uint32_t src = ds1 & ram.getAddrMask();
    uint32_t len = ds0 & ram.getAddrMask();

    uint8_t c1, c2, x;
    c1 = 0;
    c2 = 0;
    while (len--) {
        x = c2 ^ ram.readU8(src++);
        c2 = crc2[x] ^ c1;
        c1 = crc1[x];
    }
    return c1 + (c2 << 8);
}

void LavaProc::lava_op_encrypt(uint32_t ds0, uint32_t ds1, uint32_t ds2)
{
    uint32_t str2 = ds0 & ram.getAddrMask();
    uint32_t len  = ds1 & ram.getAddrMask();
    uint32_t str1 = ds2 & ram.getAddrMask();

    uint32_t i = 0;
    while (len--) {
        uint8_t c = ram.readU8(str2 + i);
        i++;
        if (c == 0) {
            c = ram.readU8(str2);
            i = 1;
        }
        ram.writeU8(str1, ram.readU8(str1) ^ c);
        str1++;
    }
}

void LavaProc::lava_op_gettime(uint32_t ds0)
{
    uint8_t *p = ram.data().data() + (ds0 & ram.getAddrMask());
    const auto &time = cb->getTime();
    p[0] = time.year;
    p[1] = time.year >> 8;
    p[2] = time.month;
    p[3] = time.day;
    p[4] = time.hour;
    p[5] = time.minute;
    p[6] = time.second;
    p[7] = time.dayOfWeek;
}

void LavaProc::lava_op_xdraw(uint32_t ds0)
{
    disp->xdraw(ds0);
}

uint32_t LavaProc::lava_op_gettick()
{
    int32_t ms = cb->getMs();
    int32_t tick = (ms % 1000) * 256 / 1000;
    //std::cerr << __func__ << ": " << ms << ", " << tick << std::endl;
    return tick;
}

uint32_t LavaProc::lava_op_checkkey(uint32_t ds0)
{
    return cb->checkKey(ds0);
}

void LavaProc::lava_op_releasekey(uint32_t ds0)
{
    cb->releaseKey(ds0);
}

void LavaProc::lava_op_getblock(uint32_t ds0, uint32_t ds1, uint32_t ds2, uint32_t ds3, uint32_t ds4, uint32_t ds5)
{
    int32_t dst = ds0;
    int32_t cfg = ds1;
    uint16_t h = ds2;
    uint16_t w = ds3;
    uint16_t y = ds4;
    uint16_t x = ds5;

    ram.writeData(dst, disp->getBlock(x, y, w, h, cfg));
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
