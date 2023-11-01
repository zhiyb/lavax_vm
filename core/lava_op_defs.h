
inline void lava_wrap_push_u8(const uint8_t *data);
uint32_t lava_op_push_u8(uint8_t dp0);

inline void lava_wrap_push_i16(const uint8_t *data);
uint32_t lava_op_push_i16(int16_t dp0);

inline void lava_wrap_push_i32(const uint8_t *data);
uint32_t lava_op_push_i32(int32_t dp0);

inline void lava_wrap_pushv_u8(const uint8_t *data);
uint32_t lava_op_pushv_u8(uint32_t dp0);

inline void lava_wrap_pushv_i16(const uint8_t *data);
uint32_t lava_op_pushv_i16(uint32_t dp0);

inline void lava_wrap_pushv_i32(const uint8_t *data);
uint32_t lava_op_pushv_i32(uint32_t dp0);

inline void lava_wrap_pushg_u8(const uint8_t *data);
uint32_t lava_op_pushg_u8(uint32_t dp0, uint32_t ds0);

inline void lava_wrap_pushg_i16(const uint8_t *data);
uint32_t lava_op_pushg_i16(uint32_t dp0, uint32_t ds0);

inline void lava_wrap_pushg_i32(const uint8_t *data);
uint32_t lava_op_pushg_i32(uint32_t dp0, uint32_t ds0);

inline void lava_wrap_pusha_u8(const uint8_t *data);
uint32_t lava_op_pusha_u8(uint32_t dp0, uint32_t ds0);

inline void lava_wrap_push_str(const uint8_t *data);
uint32_t lava_op_push_str(const std::string &dp0);

inline void lava_wrap_pusha_i32(const uint8_t *data);
uint32_t lava_op_pusha_i32(uint32_t dp0, uint32_t ds0);

inline void lava_wrap_INC(const uint8_t *data);
void lava_op_INC(uint32_t ds0);

inline void lava_wrap_DEC(const uint8_t *data);
void lava_op_DEC(uint32_t ds0);

inline void lava_wrap_add(const uint8_t *data);
uint32_t lava_op_add(uint32_t ds0, uint32_t ds1);

inline void lava_wrap_sub(const uint8_t *data);
uint32_t lava_op_sub(uint32_t ds0, uint32_t ds1);

inline void lava_wrap_land(const uint8_t *data);
uint32_t lava_op_land(uint32_t ds0, uint32_t ds1);

inline void lava_wrap_lor(const uint8_t *data);
uint32_t lava_op_lor(uint32_t ds0, uint32_t ds1);

inline void lava_wrap_mul(const uint8_t *data);
uint32_t lava_op_mul(uint32_t ds0, uint32_t ds1);

inline void lava_wrap_div(const uint8_t *data);
uint32_t lava_op_div(uint32_t ds0, uint32_t ds1);

inline void lava_wrap_equ(const uint8_t *data);
uint32_t lava_op_equ(uint32_t ds0, uint32_t ds1);

inline void lava_wrap_le(const uint8_t *data);
uint32_t lava_op_le(uint32_t ds0, uint32_t ds1);

inline void lava_wrap_ge(const uint8_t *data);
uint32_t lava_op_ge(uint32_t ds0, uint32_t ds1);

inline void lava_wrap_great(const uint8_t *data);
uint32_t lava_op_great(uint32_t ds0, uint32_t ds1);

inline void lava_wrap_less(const uint8_t *data);
uint32_t lava_op_less(uint32_t ds0, uint32_t ds1);

inline void lava_wrap_let(const uint8_t *data);
uint32_t lava_op_let(uint32_t ds0, uint32_t ds1);

inline void lava_wrap_pop(const uint8_t *data);
void lava_op_pop(uint32_t ds0);

inline void lava_wrap_jmpe(const uint8_t *data);
void lava_op_jmpe(uint32_t dp0);

inline void lava_wrap_jmp(const uint8_t *data);
void lava_op_jmp(uint32_t dp0);

inline void lava_wrap_set_sp(const uint8_t *data);
void lava_op_set_sp(uint32_t dp0);

inline void lava_wrap_add_bp(const uint8_t *data);
void lava_op_add_bp(uint32_t dp0, uint8_t dp1);

inline void lava_wrap_quit(const uint8_t *data);
void lava_op_quit();

inline void lava_wrap_qadd(const uint8_t *data);
uint32_t lava_op_qadd(int16_t dp0, uint32_t ds0);

inline void lava_wrap_qsub(const uint8_t *data);
uint32_t lava_op_qsub(int16_t dp0, uint32_t ds0);

inline void lava_wrap_qmul(const uint8_t *data);
uint32_t lava_op_qmul(int16_t dp0, uint32_t ds0);

inline void lava_wrap_qdiv(const uint8_t *data);
uint32_t lava_op_qdiv(int16_t dp0, uint32_t ds0);

inline void lava_wrap_qlshift(const uint8_t *data);
uint32_t lava_op_qlshift(int16_t dp0, uint32_t ds0);

inline void lava_wrap_qequ(const uint8_t *data);
uint32_t lava_op_qequ(int16_t dp0, uint32_t ds0);

inline void lava_wrap_qneq(const uint8_t *data);
uint32_t lava_op_qneq(int16_t dp0, uint32_t ds0);

inline void lava_wrap_qgreat(const uint8_t *data);
uint32_t lava_op_qgreat(int16_t dp0, uint32_t ds0);

inline void lava_wrap_qless(const uint8_t *data);
uint32_t lava_op_qless(int16_t dp0, uint32_t ds0);

inline void lava_wrap_getchar(const uint8_t *data);
void lava_op_getchar();

inline void lava_wrap_strlen(const uint8_t *data);
uint32_t lava_op_strlen(uint32_t ds0);

inline void lava_wrap_delay(const uint8_t *data);
void lava_op_delay(uint32_t ds0);

inline void lava_wrap_writeblock(const uint8_t *data);
void lava_op_writeblock();

inline void lava_wrap_scroll(const uint8_t *data);
void lava_op_scroll();

inline void lava_wrap_textout(const uint8_t *data);
void lava_op_textout();

inline void lava_wrap_block(const uint8_t *data);
void lava_op_block();

inline void lava_wrap_rectangle(const uint8_t *data);
void lava_op_rectangle();

inline void lava_wrap_exit(const uint8_t *data);
void lava_op_exit(uint32_t ds0);

inline void lava_wrap_clearscreen(const uint8_t *data);
void lava_op_clearscreen();

inline void lava_wrap_fopen(const uint8_t *data);
uint32_t lava_op_fopen(uint32_t ds0, uint32_t ds1);

inline void lava_wrap_fclose(const uint8_t *data);
void lava_op_fclose(uint32_t ds0);

inline void lava_wrap_fread(const uint8_t *data);
void lava_op_fread();

inline void lava_wrap_fwrite(const uint8_t *data);
void lava_op_fwrite();

inline void lava_wrap_sprintf(const uint8_t *data);
void lava_op_sprintf();

inline void lava_wrap_checkkey(const uint8_t *data);
void lava_op_checkkey();

inline void lava_wrap_setgraphmode(const uint8_t *data);
uint32_t lava_op_setgraphmode(uint32_t ds0);

inline void lava_wrap_setbgcolor(const uint8_t *data);
void lava_op_setbgcolor(uint32_t ds0);

inline void lava_wrap_setfgcolor(const uint8_t *data);
void lava_op_setfgcolor(uint32_t ds0);
