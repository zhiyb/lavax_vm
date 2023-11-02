#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>

#include "lava.h"
#include "lava_proc.h"

#define TODO()  throw std::runtime_error("PROC_TODO: " + std::to_string(__LINE__))


bool LavaProc::load(const std::vector<uint8_t> &source, uint32_t rambits, bool pen_input)
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

#if 1
    std::cerr << "OP code:" << std::endl;

    for (auto const &op: op_exec) {
        std::cerr << std::showbase << std::internal << std::setfill('0');
        std::cerr << std::hex << std::setw(10) << op.first << ": ";
        std::cerr << op_info[op.second.opcode].name; // << std::endl;

        std::stringstream dss;
        for (auto const &v: op.second.data) {
            dss << ", 0x";
            dss << std::hex << std::setfill('0') << std::setw(2) << (uint32_t)v;
        }
        std::cerr << dss.str() << std::endl;
    }
#endif

    return true;
}

uint32_t LavaProc::parse(uint32_t ofs)
{
    uint8_t opcode = source[ofs];
    op_info_t info = op_info[opcode];
    uint32_t size = info.len;
    if (info.len & OpParamAddr)
        size = (size - OpParamAddr) + (rambits <= 16 ? 2 : 3);
    if (info.len == OpParamStr) {
        uint32_t i = ofs + 1;
        while (source.at(i) != '\0')
            i++;
        size = i - ofs + 1;
    }
    op_exec[ofs] = op_t{
        info.func,
        std::vector<uint8_t>(source.begin() + ofs + 1, source.begin() + ofs + size),
        opcode,
    };
    return size;
}

LavaProc::proc_req_t LavaProc::run()
{
    auto const &op = op_exec[pc];
    LavaProc::proc_req_t req = ReqNone;

#if 1
    static bool print = true;
    if (print) {
        std::cerr << "OP execution:" << std::endl;
        print = false;
    }

    std::cerr << std::showbase << std::internal << std::setfill('0');
    std::cerr << std::hex << std::setw(10) << pc << ": ";
    std::cerr << op_info[op.opcode].name;

    std::stringstream dss;
    for (auto const &v: op.data) {
        dss << ", 0x";
        dss << std::hex << std::setfill('0') << std::setw(2) << (uint32_t)v;
    }
    std::cerr << dss.str() << std::endl;
#endif

    uint32_t pc_last = pc;
    op.func(this, op.data);
    if (pc == pc_last)
        pc = pc + 1 + op.data.size();

    if (disp->refreshRequest())
        req = (proc_req_t)(req | ReqRefresh);
    return req;
}

#include "lava_op_code.h"

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
    if (rambits <= 16)
        a = (a & 0x0000ffff) | 0x00010000;
    else
        a = (a & 0x00ffffff) | 0x01000000;
    return a;
}

uint32_t LavaProc::lava_op_push_str(const std::vector<uint8_t> &dp0)
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
    std::cerr << "PROC_TODO: " << __FUNCSIG__ << std::endl;

    int32_t a3 = ds0;
    int32_t a1 = ds1;

	int32_t t,t1;
    uint32_t local_bp = ram.getLocalStackBp();

	if (rambits>16) {
		if (a1&0x80000000) t1=(a1+local_bp)&0xffffff; //???有可能越界，但也可以不检验
		else t1=a1&0xffffff;
		t=(a1>>24)&0x7f;
	} else {
		if (a1&0x800000) t1=(a1+local_bp)&0xffff; //???有可能越界，但也可以不检验
		else t1=a1&0xffff;
		t=(a1>>16)&0x7f;
	}

	if (t==1)
        ram.writeU8(t1, a3 & 0xff);
	else if (t==2)
        ram.writeI16(t1, a3 & 0xffff);
	else
        ram.writeI32(t1, a3);

	a1=a3;
    return a1;
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
    ram.setLocalStack(dp0);
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

void LavaProc::lava_op_block(uint32_t ds0, uint32_t ds1, uint32_t ds2, uint32_t ds3, uint32_t ds4)
{
    std::cerr << "PROC_TODO: " << __FUNCSIG__ << std::endl;
    std::cerr << std::hex << ds0 << ", " << ds1 << ", " << ds2 << ", " << ds3 << ", " << ds4 << std::endl;

	uint8_t no_buf = ds0 & 0x40;
	uint8_t cmd = ds0 & 3;

	uint16_t y1 = ds1;
	uint16_t x1 = ds2;
	uint16_t y0 = ds3;
	uint16_t x0 = ds4;

    disp->drawBlock(x0, x1, y0, y1, cmd, no_buf);
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
    disp->clearScreen();
}

uint32_t LavaProc::lava_op_fopen(uint32_t ds0, uint32_t ds1)
{
    std::cerr << "PROC_TODO: " << __FUNCSIG__ << std::endl;
    return 0;
}

void LavaProc::lava_op_fclose(uint32_t ds0)
{
    std::cerr << "PROC_TODO: " << __FUNCSIG__ << std::endl;
}

uint32_t LavaProc::lava_op_fread(uint32_t ds0, uint32_t ds1, uint32_t ds2)
{
    std::cerr << "PROC_TODO: " << __FUNCSIG__ << std::endl;
    return 0;
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
    disp->setBackgroundColour(ds0);
}

void LavaProc::lava_op_setfgcolor(uint32_t ds0)
{
    disp->setForegroundColour(ds0);
}
