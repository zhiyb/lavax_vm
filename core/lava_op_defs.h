
void lava_wrap_push_u8(const std::vector<uint8_t> &data);
uint32_t lava_op_push_u8(uint8_t dp0);

void lava_wrap_push_i16(const std::vector<uint8_t> &data);
uint32_t lava_op_push_i16(int16_t dp0);

void lava_wrap_push_i32(const std::vector<uint8_t> &data);
uint32_t lava_op_push_i32(int32_t dp0);

void lava_wrap_pushv_u8(const std::vector<uint8_t> &data);
uint32_t lava_op_pushv_u8(uint32_t dp0);

void lava_wrap_pushv_i16(const std::vector<uint8_t> &data);
uint32_t lava_op_pushv_i16(uint32_t dp0);

void lava_wrap_pushv_i32(const std::vector<uint8_t> &data);
uint32_t lava_op_pushv_i32(uint32_t dp0);

void lava_wrap_pushg_u8(const std::vector<uint8_t> &data);
uint32_t lava_op_pushg_u8(uint32_t dp0, uint32_t ds0);

void lava_wrap_pushg_i16(const std::vector<uint8_t> &data);
uint32_t lava_op_pushg_i16(uint32_t dp0, uint32_t ds0);

void lava_wrap_pushg_i32(const std::vector<uint8_t> &data);
uint32_t lava_op_pushg_i32(uint32_t dp0, uint32_t ds0);

void lava_wrap_pusha_u8(const std::vector<uint8_t> &data);
uint32_t lava_op_pusha_u8(uint32_t dp0, uint32_t ds0);

void lava_wrap_push_str(const std::vector<uint8_t> &data);
uint32_t lava_op_push_str(const std::vector<uint8_t> &dp0);

void lava_wrap_pusha_i32(const std::vector<uint8_t> &data);
uint32_t lava_op_pusha_i32(uint32_t dp0, uint32_t ds0);

void lava_wrap_INC(const std::vector<uint8_t> &data);
void lava_op_INC(uint32_t ds0);

void lava_wrap_DEC(const std::vector<uint8_t> &data);
void lava_op_DEC(uint32_t ds0);

void lava_wrap_add(const std::vector<uint8_t> &data);
uint32_t lava_op_add(uint32_t ds0, uint32_t ds1);

void lava_wrap_sub(const std::vector<uint8_t> &data);
uint32_t lava_op_sub(uint32_t ds0, uint32_t ds1);

void lava_wrap_land(const std::vector<uint8_t> &data);
uint32_t lava_op_land(uint32_t ds0, uint32_t ds1);

void lava_wrap_lor(const std::vector<uint8_t> &data);
uint32_t lava_op_lor(uint32_t ds0, uint32_t ds1);

void lava_wrap_mul(const std::vector<uint8_t> &data);
uint32_t lava_op_mul(uint32_t ds0, uint32_t ds1);

void lava_wrap_div(const std::vector<uint8_t> &data);
uint32_t lava_op_div(uint32_t ds0, uint32_t ds1);

void lava_wrap_equ(const std::vector<uint8_t> &data);
uint32_t lava_op_equ(uint32_t ds0, uint32_t ds1);

void lava_wrap_le(const std::vector<uint8_t> &data);
uint32_t lava_op_le(uint32_t ds0, uint32_t ds1);

void lava_wrap_ge(const std::vector<uint8_t> &data);
uint32_t lava_op_ge(uint32_t ds0, uint32_t ds1);

void lava_wrap_great(const std::vector<uint8_t> &data);
uint32_t lava_op_great(uint32_t ds0, uint32_t ds1);

void lava_wrap_less(const std::vector<uint8_t> &data);
uint32_t lava_op_less(uint32_t ds0, uint32_t ds1);

void lava_wrap_let(const std::vector<uint8_t> &data);
uint32_t lava_op_let(uint32_t ds0, uint32_t ds1);

void lava_wrap_pop(const std::vector<uint8_t> &data);
void lava_op_pop(uint32_t ds0);

void lava_wrap_jmpe(const std::vector<uint8_t> &data);
void lava_op_jmpe(uint32_t dp0);

void lava_wrap_jmp(const std::vector<uint8_t> &data);
void lava_op_jmp(uint32_t dp0);

void lava_wrap_set_sp(const std::vector<uint8_t> &data);
void lava_op_set_sp(uint32_t dp0);

void lava_wrap_add_bp(const std::vector<uint8_t> &data);
void lava_op_add_bp(uint32_t dp0, uint8_t dp1);

void lava_wrap_quit(const std::vector<uint8_t> &data);
void lava_op_quit();

void lava_wrap_qadd(const std::vector<uint8_t> &data);
uint32_t lava_op_qadd(int16_t dp0, uint32_t ds0);

void lava_wrap_qsub(const std::vector<uint8_t> &data);
uint32_t lava_op_qsub(int16_t dp0, uint32_t ds0);

void lava_wrap_qmul(const std::vector<uint8_t> &data);
uint32_t lava_op_qmul(int16_t dp0, uint32_t ds0);

void lava_wrap_qdiv(const std::vector<uint8_t> &data);
uint32_t lava_op_qdiv(int16_t dp0, uint32_t ds0);

void lava_wrap_qlshift(const std::vector<uint8_t> &data);
uint32_t lava_op_qlshift(int16_t dp0, uint32_t ds0);

void lava_wrap_qequ(const std::vector<uint8_t> &data);
uint32_t lava_op_qequ(int16_t dp0, uint32_t ds0);

void lava_wrap_qneq(const std::vector<uint8_t> &data);
uint32_t lava_op_qneq(int16_t dp0, uint32_t ds0);

void lava_wrap_qgreat(const std::vector<uint8_t> &data);
uint32_t lava_op_qgreat(int16_t dp0, uint32_t ds0);

void lava_wrap_qless(const std::vector<uint8_t> &data);
uint32_t lava_op_qless(int16_t dp0, uint32_t ds0);

void lava_wrap_getchar(const std::vector<uint8_t> &data);
void lava_op_getchar();

void lava_wrap_strlen(const std::vector<uint8_t> &data);
uint32_t lava_op_strlen(uint32_t ds0);

void lava_wrap_delay(const std::vector<uint8_t> &data);
void lava_op_delay(uint32_t ds0);

void lava_wrap_writeblock(const std::vector<uint8_t> &data);
void lava_op_writeblock();

void lava_wrap_scroll(const std::vector<uint8_t> &data);
void lava_op_scroll();

void lava_wrap_textout(const std::vector<uint8_t> &data);
void lava_op_textout();

void lava_wrap_block(const std::vector<uint8_t> &data);
void lava_op_block(uint32_t ds0, uint32_t ds1, uint32_t ds2, uint32_t ds3, uint32_t ds4);

void lava_wrap_rectangle(const std::vector<uint8_t> &data);
void lava_op_rectangle();

void lava_wrap_exit(const std::vector<uint8_t> &data);
void lava_op_exit(uint32_t ds0);

void lava_wrap_clearscreen(const std::vector<uint8_t> &data);
void lava_op_clearscreen();

void lava_wrap_fopen(const std::vector<uint8_t> &data);
uint32_t lava_op_fopen(uint32_t ds0, uint32_t ds1);

void lava_wrap_fclose(const std::vector<uint8_t> &data);
void lava_op_fclose(uint32_t ds0);

void lava_wrap_fread(const std::vector<uint8_t> &data);
uint32_t lava_op_fread(uint32_t ds0, uint32_t ds1, uint32_t ds2);

void lava_wrap_fwrite(const std::vector<uint8_t> &data);
void lava_op_fwrite();

void lava_wrap_sprintf(const std::vector<uint8_t> &data);
void lava_op_sprintf();

void lava_wrap_checkkey(const std::vector<uint8_t> &data);
void lava_op_checkkey();

void lava_wrap_setgraphmode(const std::vector<uint8_t> &data);
uint32_t lava_op_setgraphmode(uint32_t ds0);

void lava_wrap_setbgcolor(const std::vector<uint8_t> &data);
void lava_op_setbgcolor(uint32_t ds0);

void lava_wrap_setfgcolor(const std::vector<uint8_t> &data);
void lava_op_setfgcolor(uint32_t ds0);
