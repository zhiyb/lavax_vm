
void LavaProc::lava_wrap_push_u8(const uint8_t *data)
{
    uint8_t dp0 = data[0];
    uint32_t ret = lava_op_push_u8(dp0);
    ram.push(ret);
}

void LavaProc::lava_wrap_push_i16(const uint8_t *data)
{
    int16_t dp0 = (int16_t)(data[0] | (data[1] << 8));
    uint32_t ret = lava_op_push_i16(dp0);
    ram.push(ret);
}

void LavaProc::lava_wrap_push_i32(const uint8_t *data)
{
    int32_t dp0 = (int32_t)(data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24));
    uint32_t ret = lava_op_push_i32(dp0);
    ram.push(ret);
}

void LavaProc::lava_wrap_pushv_u8(const uint8_t *data)
{
    uint32_t dp0;
    if (rambits <= 16) {
        dp0 = (uint32_t)(data[0] | (data[1] << 8));
    } else {
        dp0 = (uint32_t)(data[0] | (data[1] << 8) | (data[2] << 16));
    }
    uint32_t ret = lava_op_pushv_u8(dp0);
    ram.push(ret);
}

void LavaProc::lava_wrap_pushv_i16(const uint8_t *data)
{
    uint32_t dp0;
    if (rambits <= 16) {
        dp0 = (uint32_t)(data[0] | (data[1] << 8));
    } else {
        dp0 = (uint32_t)(data[0] | (data[1] << 8) | (data[2] << 16));
    }
    uint32_t ret = lava_op_pushv_i16(dp0);
    ram.push(ret);
}

void LavaProc::lava_wrap_pushv_i32(const uint8_t *data)
{
    uint32_t dp0;
    if (rambits <= 16) {
        dp0 = (uint32_t)(data[0] | (data[1] << 8));
    } else {
        dp0 = (uint32_t)(data[0] | (data[1] << 8) | (data[2] << 16));
    }
    uint32_t ret = lava_op_pushv_i32(dp0);
    ram.push(ret);
}

void LavaProc::lava_wrap_pushg_u8(const uint8_t *data)
{
    uint32_t dp0;
    if (rambits <= 16) {
        dp0 = (uint32_t)(data[0] | (data[1] << 8));
    } else {
        dp0 = (uint32_t)(data[0] | (data[1] << 8) | (data[2] << 16));
    }
    uint32_t ds0 = ram.pop();
    uint32_t ret = lava_op_pushg_u8(dp0, ds0);
    ram.push(ret);
}

void LavaProc::lava_wrap_pushg_i16(const uint8_t *data)
{
    uint32_t dp0;
    if (rambits <= 16) {
        dp0 = (uint32_t)(data[0] | (data[1] << 8));
    } else {
        dp0 = (uint32_t)(data[0] | (data[1] << 8) | (data[2] << 16));
    }
    uint32_t ds0 = ram.pop();
    uint32_t ret = lava_op_pushg_i16(dp0, ds0);
    ram.push(ret);
}

void LavaProc::lava_wrap_pushg_i32(const uint8_t *data)
{
    uint32_t dp0;
    if (rambits <= 16) {
        dp0 = (uint32_t)(data[0] | (data[1] << 8));
    } else {
        dp0 = (uint32_t)(data[0] | (data[1] << 8) | (data[2] << 16));
    }
    uint32_t ds0 = ram.pop();
    uint32_t ret = lava_op_pushg_i32(dp0, ds0);
    ram.push(ret);
}

void LavaProc::lava_wrap_pusha_u8(const uint8_t *data)
{
    uint32_t dp0;
    if (rambits <= 16) {
        dp0 = (uint32_t)(data[0] | (data[1] << 8));
    } else {
        dp0 = (uint32_t)(data[0] | (data[1] << 8) | (data[2] << 16));
    }
    uint32_t ds0 = ram.pop();
    uint32_t ret = lava_op_pusha_u8(dp0, ds0);
    ram.push(ret);
}

void LavaProc::lava_wrap_push_str(const uint8_t *data)
{
    const std::string &dp0 = param_string(data);
    uint32_t ret = lava_op_push_str(dp0);
    ram.push(ret);
}

void LavaProc::lava_wrap_pusha_i32(const uint8_t *data)
{
    uint32_t dp0;
    if (rambits <= 16) {
        dp0 = (uint32_t)(data[0] | (data[1] << 8));
    } else {
        dp0 = (uint32_t)(data[0] | (data[1] << 8) | (data[2] << 16));
    }
    uint32_t ds0 = ram.pop();
    uint32_t ret = lava_op_pusha_i32(dp0, ds0);
    ram.push(ret);
}

void LavaProc::lava_wrap_INC(const uint8_t *data)
{
    uint32_t ds0 = ram.pop();
    lava_op_INC(ds0);
    ;
}

void LavaProc::lava_wrap_DEC(const uint8_t *data)
{
    uint32_t ds0 = ram.pop();
    lava_op_DEC(ds0);
    ;
}

void LavaProc::lava_wrap_add(const uint8_t *data)
{
    uint32_t ds0 = ram.pop();
    uint32_t ds1 = ram.pop();
    uint32_t ret = lava_op_add(ds0, ds1);
    ram.push(ret);
}

void LavaProc::lava_wrap_sub(const uint8_t *data)
{
    uint32_t ds0 = ram.pop();
    uint32_t ds1 = ram.pop();
    uint32_t ret = lava_op_sub(ds0, ds1);
    ram.push(ret);
}

void LavaProc::lava_wrap_land(const uint8_t *data)
{
    uint32_t ds0 = ram.pop();
    uint32_t ds1 = ram.pop();
    uint32_t ret = lava_op_land(ds0, ds1);
    ram.push(ret);
}

void LavaProc::lava_wrap_lor(const uint8_t *data)
{
    uint32_t ds0 = ram.pop();
    uint32_t ds1 = ram.pop();
    uint32_t ret = lava_op_lor(ds0, ds1);
    ram.push(ret);
}

void LavaProc::lava_wrap_mul(const uint8_t *data)
{
    uint32_t ds0 = ram.pop();
    uint32_t ds1 = ram.pop();
    uint32_t ret = lava_op_mul(ds0, ds1);
    ram.push(ret);
}

void LavaProc::lava_wrap_div(const uint8_t *data)
{
    uint32_t ds0 = ram.pop();
    uint32_t ds1 = ram.pop();
    uint32_t ret = lava_op_div(ds0, ds1);
    ram.push(ret);
}

void LavaProc::lava_wrap_equ(const uint8_t *data)
{
    uint32_t ds0 = ram.pop();
    uint32_t ds1 = ram.pop();
    uint32_t ret = lava_op_equ(ds0, ds1);
    ram.push(ret);
}

void LavaProc::lava_wrap_le(const uint8_t *data)
{
    uint32_t ds0 = ram.pop();
    uint32_t ds1 = ram.pop();
    uint32_t ret = lava_op_le(ds0, ds1);
    ram.push(ret);
}

void LavaProc::lava_wrap_ge(const uint8_t *data)
{
    uint32_t ds0 = ram.pop();
    uint32_t ds1 = ram.pop();
    uint32_t ret = lava_op_ge(ds0, ds1);
    ram.push(ret);
}

void LavaProc::lava_wrap_great(const uint8_t *data)
{
    uint32_t ds0 = ram.pop();
    uint32_t ds1 = ram.pop();
    uint32_t ret = lava_op_great(ds0, ds1);
    ram.push(ret);
}

void LavaProc::lava_wrap_less(const uint8_t *data)
{
    uint32_t ds0 = ram.pop();
    uint32_t ds1 = ram.pop();
    uint32_t ret = lava_op_less(ds0, ds1);
    ram.push(ret);
}

void LavaProc::lava_wrap_let(const uint8_t *data)
{
    uint32_t ds0 = ram.pop();
    uint32_t ds1 = ram.pop();
    uint32_t ret = lava_op_let(ds0, ds1);
    ram.push(ret);
}

void LavaProc::lava_wrap_pop(const uint8_t *data)
{
    uint32_t ds0 = ram.pop();
    lava_op_pop(ds0);
    ;
}

void LavaProc::lava_wrap_jmpe(const uint8_t *data)
{
    uint32_t dp0 = (uint32_t)(data[0] | (data[1] << 8) | (data[2] << 16));
    lava_op_jmpe(dp0);
    ;
}

void LavaProc::lava_wrap_jmp(const uint8_t *data)
{
    uint32_t dp0 = (uint32_t)(data[0] | (data[1] << 8) | (data[2] << 16));
    lava_op_jmp(dp0);
    ;
}

void LavaProc::lava_wrap_set_sp(const uint8_t *data)
{
    uint32_t dp0;
    if (rambits <= 16) {
        dp0 = (uint32_t)(data[0] | (data[1] << 8));
    } else {
        dp0 = (uint32_t)(data[0] | (data[1] << 8) | (data[2] << 16));
    }
    lava_op_set_sp(dp0);
    ;
}

void LavaProc::lava_wrap_add_bp(const uint8_t *data)
{
    uint32_t dp0;
    uint8_t dp1;
    if (rambits <= 16) {
        dp0 = (uint32_t)(data[0] | (data[1] << 8));
        dp1 = data[2];
    } else {
        dp0 = (uint32_t)(data[0] | (data[1] << 8) | (data[2] << 16));
        dp1 = data[3];
    }
    lava_op_add_bp(dp0, dp1);
    ;
}

void LavaProc::lava_wrap_quit(const uint8_t *data)
{

    lava_op_quit();
    ;
}

void LavaProc::lava_wrap_qadd(const uint8_t *data)
{
    int16_t dp0 = (int16_t)(data[0] | (data[1] << 8));
    uint32_t ds0 = ram.pop();
    uint32_t ret = lava_op_qadd(dp0, ds0);
    ram.push(ret);
}

void LavaProc::lava_wrap_qsub(const uint8_t *data)
{
    int16_t dp0 = (int16_t)(data[0] | (data[1] << 8));
    uint32_t ds0 = ram.pop();
    uint32_t ret = lava_op_qsub(dp0, ds0);
    ram.push(ret);
}

void LavaProc::lava_wrap_qmul(const uint8_t *data)
{
    int16_t dp0 = (int16_t)(data[0] | (data[1] << 8));
    uint32_t ds0 = ram.pop();
    uint32_t ret = lava_op_qmul(dp0, ds0);
    ram.push(ret);
}

void LavaProc::lava_wrap_qdiv(const uint8_t *data)
{
    int16_t dp0 = (int16_t)(data[0] | (data[1] << 8));
    uint32_t ds0 = ram.pop();
    uint32_t ret = lava_op_qdiv(dp0, ds0);
    ram.push(ret);
}

void LavaProc::lava_wrap_qlshift(const uint8_t *data)
{
    int16_t dp0 = (int16_t)(data[0] | (data[1] << 8));
    uint32_t ds0 = ram.pop();
    uint32_t ret = lava_op_qlshift(dp0, ds0);
    ram.push(ret);
}

void LavaProc::lava_wrap_qequ(const uint8_t *data)
{
    int16_t dp0 = (int16_t)(data[0] | (data[1] << 8));
    uint32_t ds0 = ram.pop();
    uint32_t ret = lava_op_qequ(dp0, ds0);
    ram.push(ret);
}

void LavaProc::lava_wrap_qneq(const uint8_t *data)
{
    int16_t dp0 = (int16_t)(data[0] | (data[1] << 8));
    uint32_t ds0 = ram.pop();
    uint32_t ret = lava_op_qneq(dp0, ds0);
    ram.push(ret);
}

void LavaProc::lava_wrap_qgreat(const uint8_t *data)
{
    int16_t dp0 = (int16_t)(data[0] | (data[1] << 8));
    uint32_t ds0 = ram.pop();
    uint32_t ret = lava_op_qgreat(dp0, ds0);
    ram.push(ret);
}

void LavaProc::lava_wrap_qless(const uint8_t *data)
{
    int16_t dp0 = (int16_t)(data[0] | (data[1] << 8));
    uint32_t ds0 = ram.pop();
    uint32_t ret = lava_op_qless(dp0, ds0);
    ram.push(ret);
}

void LavaProc::lava_wrap_getchar(const uint8_t *data)
{

    lava_op_getchar();
    ;
}

void LavaProc::lava_wrap_strlen(const uint8_t *data)
{
    uint32_t ds0 = ram.pop();
    uint32_t ret = lava_op_strlen(ds0);
    ram.push(ret);
}

void LavaProc::lava_wrap_delay(const uint8_t *data)
{
    uint32_t ds0 = ram.pop();
    lava_op_delay(ds0);
    ;
}

void LavaProc::lava_wrap_writeblock(const uint8_t *data)
{

    lava_op_writeblock();
    ;
}

void LavaProc::lava_wrap_scroll(const uint8_t *data)
{

    lava_op_scroll();
    ;
}

void LavaProc::lava_wrap_textout(const uint8_t *data)
{

    lava_op_textout();
    ;
}

void LavaProc::lava_wrap_block(const uint8_t *data)
{

    lava_op_block();
    ;
}

void LavaProc::lava_wrap_rectangle(const uint8_t *data)
{

    lava_op_rectangle();
    ;
}

void LavaProc::lava_wrap_exit(const uint8_t *data)
{
    uint32_t ds0 = ram.pop();
    lava_op_exit(ds0);
    ;
}

void LavaProc::lava_wrap_clearscreen(const uint8_t *data)
{

    lava_op_clearscreen();
    ;
}

void LavaProc::lava_wrap_fopen(const uint8_t *data)
{
    uint32_t ds0 = ram.pop();
    uint32_t ds1 = ram.pop();
    uint32_t ret = lava_op_fopen(ds0, ds1);
    ram.push(ret);
}

void LavaProc::lava_wrap_fclose(const uint8_t *data)
{
    uint32_t ds0 = ram.pop();
    lava_op_fclose(ds0);
    ;
}

void LavaProc::lava_wrap_fread(const uint8_t *data)
{

    lava_op_fread();
    ;
}

void LavaProc::lava_wrap_fwrite(const uint8_t *data)
{

    lava_op_fwrite();
    ;
}

void LavaProc::lava_wrap_sprintf(const uint8_t *data)
{

    lava_op_sprintf();
    ;
}

void LavaProc::lava_wrap_checkkey(const uint8_t *data)
{

    lava_op_checkkey();
    ;
}

void LavaProc::lava_wrap_setgraphmode(const uint8_t *data)
{
    uint32_t ds0 = ram.pop();
    uint32_t ret = lava_op_setgraphmode(ds0);
    ram.push(ret);
}

void LavaProc::lava_wrap_setbgcolor(const uint8_t *data)
{
    uint32_t ds0 = ram.pop();
    lava_op_setbgcolor(ds0);
    ;
}

void LavaProc::lava_wrap_setfgcolor(const uint8_t *data)
{
    uint32_t ds0 = ram.pop();
    lava_op_setfgcolor(ds0);
    ;
}
