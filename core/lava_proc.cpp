#include <stdexcept>

#include "lava.h"
#include "lava_proc.h"

#define TODO()  throw std::runtime_error("TODO: " + std::to_string(__LINE__))

bool LavaProc::load(const std::vector<uint8_t> &source, uint32_t rambits, bool pen_input)
{
    this->source = source;
    this->rambits = rambits;
    this->pen_input = pen_input;

    ram.init(rambits);

    pc = 16;

    return true;
}

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
    return (uint32_t)ram.getU8(dp0);
}

uint32_t LavaProc::lava_op_pushv_i16(uint32_t dp0)
{
    return (uint32_t)(int32_t)ram.getI16(dp0);
}

uint32_t LavaProc::lava_op_pushv_i32(uint32_t dp0)
{
    return (uint32_t)ram.getI32(dp0);
}

uint32_t LavaProc::lava_op_pushg_u8(uint32_t dp0, uint32_t ds0)
{
    return (uint32_t)ram.getU8(dp0 + ds0);
}

uint32_t LavaProc::lava_op_pushg_i16(uint32_t dp0, uint32_t ds0)
{
    return (uint32_t)(int32_t)ram.getI16(dp0 + ds0);
}

uint32_t LavaProc::lava_op_pushg_i32(uint32_t dp0, uint32_t ds0)
{
    return (uint32_t)ram.getI32(dp0 + ds0);
}

uint32_t LavaProc::lava_op_pusha_u8(uint32_t dp0, uint32_t ds0)
{
    uint32_t a = dp0 + ds0;
    if (rambits <= 16)
        a = (a & 0x0000ffff) | 0x00010000;
    else
        a = (a & 0x00ffffff) | 0x01000000;
    return a;
}

uint32_t LavaProc::lava_op_push_str(const std::string &dp0)
{
    return ram.pushString(dp0);
}

uint32_t LavaProc::lava_op_pusha_i32(uint32_t dp0, uint32_t ds0)
{
    uint32_t a = dp0 + ds0;
    if (rambits <= 16)
        a = a & 0x0000ffff;
    else
        a = a & 0x00ffffff;
    return a;
}

void LavaProc::lava_op_INC(uint32_t ds0)
{
    TODO();
}

void LavaProc::lava_op_DEC(uint32_t ds0)
{
    TODO();
}

uint32_t LavaProc::lava_op_add(uint32_t ds0, uint32_t ds1)
{
    return (uint32_t)((int32_t)ds1 + (int32_t)ds0);
}

uint32_t LavaProc::lava_op_sub(uint32_t ds0, uint32_t ds1)
{
    return (uint32_t)((int32_t)ds1 - (int32_t)ds0);
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

uint32_t LavaProc::lava_op_equ(uint32_t ds0, uint32_t ds1)
{
    return ((int32_t)ds1 == (int32_t)ds0) ? Lava::True : Lava::False;
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
    TODO();
    return 0;
}

void LavaProc::lava_op_pop(uint32_t ds0)
{
    flagv = ds0;
}

void LavaProc::lava_op_jmpe(uint32_t ds0)
{
    if (flagv)
        pc = ds0;
}

void LavaProc::lava_op_jmp(uint32_t ds0)
{
    pc = ds0;
}

void LavaProc::lava_op_set_sp(uint32_t dp0)
{
    TODO();
}

void LavaProc::lava_op_add_bp(uint32_t dp0, uint8_t dp1)
{
    TODO();
}

void LavaProc::lava_op_quit()
{
    TODO();
}

uint32_t LavaProc::lava_op_qadd(int16_t dp0, uint32_t ds0)
{
    return (uint32_t)((int32_t)ds0 + (int32_t)dp0);
}

uint32_t LavaProc::lava_op_qsub(int16_t dp0, uint32_t ds0)
{
    return (uint32_t)((int32_t)ds0 - (int32_t)dp0);
}

uint32_t LavaProc::lava_op_qmul(int16_t dp0, uint32_t ds0)
{
    return (uint32_t)((int32_t)ds0 * (int32_t)dp0);
}

uint32_t LavaProc::lava_op_qdiv(int16_t dp0, uint32_t ds0)
{
    return (uint32_t)((int32_t)ds0 / (int32_t)dp0);
}

uint32_t LavaProc::lava_op_qlshift(int16_t dp0, uint32_t ds0)
{
    TODO();
    return 0;
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

void LavaProc::lava_op_getchar()
{
    TODO();
}

uint32_t LavaProc::lava_op_strlen(uint32_t ds0)
{
    TODO();
    return 0;
}

void LavaProc::lava_op_delay(uint32_t ds0)
{
    uint32_t delay_ms = ds0 > 0x7fff ? 0x7fff : ds0;
    TODO();
}

void LavaProc::lava_op_writeblock()
{
    TODO();
}

void LavaProc::lava_op_scroll()
{
    TODO();
}

void LavaProc::lava_op_textout()
{
    TODO();
}

void LavaProc::lava_op_block()
{
    TODO();
}

void LavaProc::lava_op_rectangle()
{
    TODO();
}

void LavaProc::lava_op_exit(uint32_t ds0)
{
    TODO();
}

void LavaProc::lava_op_clearscreen()
{
    TODO();
}

uint32_t LavaProc::lava_op_fopen(uint32_t ds0, uint32_t ds1)
{
    TODO();
    return 0;
}

void LavaProc::lava_op_fclose(uint32_t ds0)
{
    TODO();
}

void LavaProc::lava_op_fread()
{
    TODO();
}

void LavaProc::lava_op_fwrite()
{
    TODO();
}

void LavaProc::lava_op_sprintf()
{
    TODO();
}

void LavaProc::lava_op_checkkey()
{
    TODO();
}

uint32_t LavaProc::lava_op_setgraphmode(uint32_t ds0)
{
    LavaDisp::mode_t mode = disp->getMode();
    disp->setMode((LavaDisp::mode_t)ds0);
    return (uint32_t)mode;
}

void LavaProc::lava_op_setbgcolor(uint32_t ds0)
{
    TODO();
}

void LavaProc::lava_op_setfgcolor(uint32_t ds0)
{
    TODO();
}
