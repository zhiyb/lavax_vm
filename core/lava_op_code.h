
void LavaProc::lava_wrap_push_u8(const std::vector<uint8_t> &data)
{
    uint8_t dp0 = data[0];
    uint32_t ret = lava_op_push_u8(dp0);
    ram.push(ret);
}

void LavaProc::lava_wrap_push_i16(const std::vector<uint8_t> &data)
{
    int16_t dp0 = (int16_t)(data[0] | (data[1] << 8));
    uint32_t ret = lava_op_push_i16(dp0);
    ram.push(ret);
}

void LavaProc::lava_wrap_push_i32(const std::vector<uint8_t> &data)
{
    int32_t dp0 = (int32_t)(data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24));
    uint32_t ret = lava_op_push_i32(dp0);
    ram.push(ret);
}

void LavaProc::lava_wrap_pushv_u8(const std::vector<uint8_t> &data)
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

void LavaProc::lava_wrap_pushv_i16(const std::vector<uint8_t> &data)
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

void LavaProc::lava_wrap_pushv_i32(const std::vector<uint8_t> &data)
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

void LavaProc::lava_wrap_pushg_u8(const std::vector<uint8_t> &data)
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

void LavaProc::lava_wrap_pushg_i16(const std::vector<uint8_t> &data)
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

void LavaProc::lava_wrap_pushg_i32(const std::vector<uint8_t> &data)
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

void LavaProc::lava_wrap_pusha_u8(const std::vector<uint8_t> &data)
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

void LavaProc::lava_wrap_push_str(const std::vector<uint8_t> &data)
{
    const std::vector<uint8_t> &dp0 = data;
    uint32_t ret = lava_op_push_str(dp0);
    ram.push(ret);
}

void LavaProc::lava_wrap_pusha_i32(const std::vector<uint8_t> &data)
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

void LavaProc::lava_wrap_INC(const std::vector<uint8_t> &data)
{
    uint32_t ds0 = ram.pop();
    lava_op_INC(ds0);
    ;
}

void LavaProc::lava_wrap_DEC(const std::vector<uint8_t> &data)
{
    uint32_t ds0 = ram.pop();
    lava_op_DEC(ds0);
    ;
}

void LavaProc::lava_wrap_add(const std::vector<uint8_t> &data)
{
    uint32_t ds0 = ram.pop();
    uint32_t ds1 = ram.pop();
    uint32_t ret = lava_op_add(ds0, ds1);
    ram.push(ret);
}

void LavaProc::lava_wrap_sub(const std::vector<uint8_t> &data)
{
    uint32_t ds0 = ram.pop();
    uint32_t ds1 = ram.pop();
    uint32_t ret = lava_op_sub(ds0, ds1);
    ram.push(ret);
}

void LavaProc::lava_wrap_land(const std::vector<uint8_t> &data)
{
    uint32_t ds0 = ram.pop();
    uint32_t ds1 = ram.pop();
    uint32_t ret = lava_op_land(ds0, ds1);
    ram.push(ret);
}

void LavaProc::lava_wrap_lor(const std::vector<uint8_t> &data)
{
    uint32_t ds0 = ram.pop();
    uint32_t ds1 = ram.pop();
    uint32_t ret = lava_op_lor(ds0, ds1);
    ram.push(ret);
}

void LavaProc::lava_wrap_mul(const std::vector<uint8_t> &data)
{
    uint32_t ds0 = ram.pop();
    uint32_t ds1 = ram.pop();
    uint32_t ret = lava_op_mul(ds0, ds1);
    ram.push(ret);
}

void LavaProc::lava_wrap_div(const std::vector<uint8_t> &data)
{
    uint32_t ds0 = ram.pop();
    uint32_t ds1 = ram.pop();
    uint32_t ret = lava_op_div(ds0, ds1);
    ram.push(ret);
}

void LavaProc::lava_wrap_equ(const std::vector<uint8_t> &data)
{
    uint32_t ds0 = ram.pop();
    uint32_t ds1 = ram.pop();
    uint32_t ret = lava_op_equ(ds0, ds1);
    ram.push(ret);
}

void LavaProc::lava_wrap_le(const std::vector<uint8_t> &data)
{
    uint32_t ds0 = ram.pop();
    uint32_t ds1 = ram.pop();
    uint32_t ret = lava_op_le(ds0, ds1);
    ram.push(ret);
}

void LavaProc::lava_wrap_ge(const std::vector<uint8_t> &data)
{
    uint32_t ds0 = ram.pop();
    uint32_t ds1 = ram.pop();
    uint32_t ret = lava_op_ge(ds0, ds1);
    ram.push(ret);
}

void LavaProc::lava_wrap_great(const std::vector<uint8_t> &data)
{
    uint32_t ds0 = ram.pop();
    uint32_t ds1 = ram.pop();
    uint32_t ret = lava_op_great(ds0, ds1);
    ram.push(ret);
}

void LavaProc::lava_wrap_less(const std::vector<uint8_t> &data)
{
    uint32_t ds0 = ram.pop();
    uint32_t ds1 = ram.pop();
    uint32_t ret = lava_op_less(ds0, ds1);
    ram.push(ret);
}

void LavaProc::lava_wrap_let(const std::vector<uint8_t> &data)
{
    uint32_t ds0 = ram.pop();
    uint32_t ds1 = ram.pop();
    uint32_t ret = lava_op_let(ds0, ds1);
    ram.push(ret);
}

void LavaProc::lava_wrap_pop(const std::vector<uint8_t> &data)
{
    uint32_t ds0 = ram.pop();
    lava_op_pop(ds0);
    ;
}

void LavaProc::lava_wrap_jmpe(const std::vector<uint8_t> &data)
{
    uint32_t dp0 = (uint32_t)(data[0] | (data[1] << 8) | (data[2] << 16));
    lava_op_jmpe(dp0);
    ;
}

void LavaProc::lava_wrap_jmp(const std::vector<uint8_t> &data)
{
    uint32_t dp0 = (uint32_t)(data[0] | (data[1] << 8) | (data[2] << 16));
    lava_op_jmp(dp0);
    ;
}

void LavaProc::lava_wrap_set_sp(const std::vector<uint8_t> &data)
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

void LavaProc::lava_wrap_add_bp(const std::vector<uint8_t> &data)
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

void LavaProc::lava_wrap_quit(const std::vector<uint8_t> &data)
{

    lava_op_quit();
    ;
}

void LavaProc::lava_wrap_qadd(const std::vector<uint8_t> &data)
{
    int16_t dp0 = (int16_t)(data[0] | (data[1] << 8));
    uint32_t ds0 = ram.pop();
    uint32_t ret = lava_op_qadd(dp0, ds0);
    ram.push(ret);
}

void LavaProc::lava_wrap_qsub(const std::vector<uint8_t> &data)
{
    int16_t dp0 = (int16_t)(data[0] | (data[1] << 8));
    uint32_t ds0 = ram.pop();
    uint32_t ret = lava_op_qsub(dp0, ds0);
    ram.push(ret);
}

void LavaProc::lava_wrap_qmul(const std::vector<uint8_t> &data)
{
    int16_t dp0 = (int16_t)(data[0] | (data[1] << 8));
    uint32_t ds0 = ram.pop();
    uint32_t ret = lava_op_qmul(dp0, ds0);
    ram.push(ret);
}

void LavaProc::lava_wrap_qdiv(const std::vector<uint8_t> &data)
{
    int16_t dp0 = (int16_t)(data[0] | (data[1] << 8));
    uint32_t ds0 = ram.pop();
    uint32_t ret = lava_op_qdiv(dp0, ds0);
    ram.push(ret);
}

void LavaProc::lava_wrap_qlshift(const std::vector<uint8_t> &data)
{
    int16_t dp0 = (int16_t)(data[0] | (data[1] << 8));
    uint32_t ds0 = ram.pop();
    uint32_t ret = lava_op_qlshift(dp0, ds0);
    ram.push(ret);
}

void LavaProc::lava_wrap_qequ(const std::vector<uint8_t> &data)
{
    int16_t dp0 = (int16_t)(data[0] | (data[1] << 8));
    uint32_t ds0 = ram.pop();
    uint32_t ret = lava_op_qequ(dp0, ds0);
    ram.push(ret);
}

void LavaProc::lava_wrap_qneq(const std::vector<uint8_t> &data)
{
    int16_t dp0 = (int16_t)(data[0] | (data[1] << 8));
    uint32_t ds0 = ram.pop();
    uint32_t ret = lava_op_qneq(dp0, ds0);
    ram.push(ret);
}

void LavaProc::lava_wrap_qgreat(const std::vector<uint8_t> &data)
{
    int16_t dp0 = (int16_t)(data[0] | (data[1] << 8));
    uint32_t ds0 = ram.pop();
    uint32_t ret = lava_op_qgreat(dp0, ds0);
    ram.push(ret);
}

void LavaProc::lava_wrap_qless(const std::vector<uint8_t> &data)
{
    int16_t dp0 = (int16_t)(data[0] | (data[1] << 8));
    uint32_t ds0 = ram.pop();
    uint32_t ret = lava_op_qless(dp0, ds0);
    ram.push(ret);
}

void LavaProc::lava_wrap_getchar(const std::vector<uint8_t> &data)
{

    lava_op_getchar();
    ;
}

void LavaProc::lava_wrap_strlen(const std::vector<uint8_t> &data)
{
    uint32_t ds0 = ram.pop();
    uint32_t ret = lava_op_strlen(ds0);
    ram.push(ret);
}

void LavaProc::lava_wrap_delay(const std::vector<uint8_t> &data)
{
    uint32_t ds0 = ram.pop();
    lava_op_delay(ds0);
    ;
}

void LavaProc::lava_wrap_writeblock(const std::vector<uint8_t> &data)
{

    lava_op_writeblock();
    ;
}

void LavaProc::lava_wrap_scroll(const std::vector<uint8_t> &data)
{

    lava_op_scroll();
    ;
}

void LavaProc::lava_wrap_textout(const std::vector<uint8_t> &data)
{
    uint32_t ds0 = ram.pop();
    uint32_t ds1 = ram.pop();
    uint32_t ds2 = ram.pop();
    uint32_t ds3 = ram.pop();
    lava_op_textout(ds0, ds1, ds2, ds3);
    ;
}

void LavaProc::lava_wrap_block(const std::vector<uint8_t> &data)
{
    uint32_t ds0 = ram.pop();
    uint32_t ds1 = ram.pop();
    uint32_t ds2 = ram.pop();
    uint32_t ds3 = ram.pop();
    uint32_t ds4 = ram.pop();
    lava_op_block(ds0, ds1, ds2, ds3, ds4);
    ;
}

void LavaProc::lava_wrap_rectangle(const std::vector<uint8_t> &data)
{

    lava_op_rectangle();
    ;
}

void LavaProc::lava_wrap_exit(const std::vector<uint8_t> &data)
{
    uint32_t ds0 = ram.pop();
    lava_op_exit(ds0);
    ;
}

void LavaProc::lava_wrap_clearscreen(const std::vector<uint8_t> &data)
{

    lava_op_clearscreen();
    ;
}

void LavaProc::lava_wrap_fopen(const std::vector<uint8_t> &data)
{
    uint32_t ds0 = ram.pop();
    uint32_t ds1 = ram.pop();
    uint32_t ret = lava_op_fopen(ds0, ds1);
    ram.push(ret);
}

void LavaProc::lava_wrap_fclose(const std::vector<uint8_t> &data)
{
    uint32_t ds0 = ram.pop();
    lava_op_fclose(ds0);
    ;
}

void LavaProc::lava_wrap_fread(const std::vector<uint8_t> &data)
{
    uint32_t ds0 = ram.pop();
    uint32_t ds1 = ram.pop();
    uint32_t ds2 = ram.pop();
    uint32_t ret = lava_op_fread(ds0, ds1, ds2);
    ram.push(ret);
}

void LavaProc::lava_wrap_fwrite(const std::vector<uint8_t> &data)
{

    lava_op_fwrite();
    ;
}

void LavaProc::lava_wrap_sprintf(const std::vector<uint8_t> &data)
{

    lava_op_sprintf();
    ;
}

void LavaProc::lava_wrap_checkkey(const std::vector<uint8_t> &data)
{

    lava_op_checkkey();
    ;
}

void LavaProc::lava_wrap_setgraphmode(const std::vector<uint8_t> &data)
{
    uint32_t ds0 = ram.pop();
    uint32_t ret = lava_op_setgraphmode(ds0);
    ram.push(ret);
}

void LavaProc::lava_wrap_setbgcolor(const std::vector<uint8_t> &data)
{
    uint32_t ds0 = ram.pop();
    lava_op_setbgcolor(ds0);
    ;
}

void LavaProc::lava_wrap_setfgcolor(const std::vector<uint8_t> &data)
{
    uint32_t ds0 = ram.pop();
    lava_op_setfgcolor(ds0);
    ;
}

std::map<uint8_t, LavaProc::op_info_t> LavaProc::op_info = {
    {0x01, op_info_t{"push_u8", OpParam1, &LavaProc::lava_wrap_push_u8}},
    {0x02, op_info_t{"push_i16", OpParam2, &LavaProc::lava_wrap_push_i16}},
    {0x03, op_info_t{"push_i32", OpParam4, &LavaProc::lava_wrap_push_i32}},
    {0x04, op_info_t{"pushv_u8", (op_len_t)(OpParam0 | OpParamAddr), &LavaProc::lava_wrap_pushv_u8}},
    {0x05, op_info_t{"pushv_i16", (op_len_t)(OpParam0 | OpParamAddr), &LavaProc::lava_wrap_pushv_i16}},
    {0x06, op_info_t{"pushv_i32", (op_len_t)(OpParam0 | OpParamAddr), &LavaProc::lava_wrap_pushv_i32}},
    {0x07, op_info_t{"pushg_u8", (op_len_t)(OpParam0 | OpParamAddr), &LavaProc::lava_wrap_pushg_u8}},
    {0x08, op_info_t{"pushg_i16", (op_len_t)(OpParam0 | OpParamAddr), &LavaProc::lava_wrap_pushg_i16}},
    {0x09, op_info_t{"pushg_i32", (op_len_t)(OpParam0 | OpParamAddr), &LavaProc::lava_wrap_pushg_i32}},
    {0x0a, op_info_t{"pusha_u8", (op_len_t)(OpParam0 | OpParamAddr), &LavaProc::lava_wrap_pusha_u8}},
    {0x0d, op_info_t{"push_str", OpParamStr, &LavaProc::lava_wrap_push_str}},
    {0x17, op_info_t{"pusha_i32", (op_len_t)(OpParam0 | OpParamAddr), &LavaProc::lava_wrap_pusha_i32}},
    {0x1f, op_info_t{"INC", OpParam0, &LavaProc::lava_wrap_INC}},
    {0x20, op_info_t{"DEC", OpParam0, &LavaProc::lava_wrap_DEC}},
    {0x21, op_info_t{"add", OpParam0, &LavaProc::lava_wrap_add}},
    {0x22, op_info_t{"sub", OpParam0, &LavaProc::lava_wrap_sub}},
    {0x27, op_info_t{"land", OpParam0, &LavaProc::lava_wrap_land}},
    {0x28, op_info_t{"lor", OpParam0, &LavaProc::lava_wrap_lor}},
    {0x2a, op_info_t{"mul", OpParam0, &LavaProc::lava_wrap_mul}},
    {0x2b, op_info_t{"div", OpParam0, &LavaProc::lava_wrap_div}},
    {0x2f, op_info_t{"equ", OpParam0, &LavaProc::lava_wrap_equ}},
    {0x31, op_info_t{"le", OpParam0, &LavaProc::lava_wrap_le}},
    {0x32, op_info_t{"ge", OpParam0, &LavaProc::lava_wrap_ge}},
    {0x33, op_info_t{"great", OpParam0, &LavaProc::lava_wrap_great}},
    {0x34, op_info_t{"less", OpParam0, &LavaProc::lava_wrap_less}},
    {0x35, op_info_t{"let", OpParam0, &LavaProc::lava_wrap_let}},
    {0x38, op_info_t{"pop", OpParam0, &LavaProc::lava_wrap_pop}},
    {0x39, op_info_t{"jmpe", OpParam3, &LavaProc::lava_wrap_jmpe}},
    {0x3b, op_info_t{"jmp", OpParam3, &LavaProc::lava_wrap_jmp}},
    {0x3c, op_info_t{"set_sp", (op_len_t)(OpParam0 | OpParamAddr), &LavaProc::lava_wrap_set_sp}},
    {0x3e, op_info_t{"add_bp", (op_len_t)(OpParam1 | OpParamAddr), &LavaProc::lava_wrap_add_bp}},
    {0x40, op_info_t{"quit", OpParam0, &LavaProc::lava_wrap_quit}},
    {0x45, op_info_t{"qadd", OpParam2, &LavaProc::lava_wrap_qadd}},
    {0x46, op_info_t{"qsub", OpParam2, &LavaProc::lava_wrap_qsub}},
    {0x47, op_info_t{"qmul", OpParam2, &LavaProc::lava_wrap_qmul}},
    {0x48, op_info_t{"qdiv", OpParam2, &LavaProc::lava_wrap_qdiv}},
    {0x4a, op_info_t{"qlshift", OpParam2, &LavaProc::lava_wrap_qlshift}},
    {0x4c, op_info_t{"qequ", OpParam2, &LavaProc::lava_wrap_qequ}},
    {0x4d, op_info_t{"qneq", OpParam2, &LavaProc::lava_wrap_qneq}},
    {0x4e, op_info_t{"qgreat", OpParam2, &LavaProc::lava_wrap_qgreat}},
    {0x4f, op_info_t{"qless", OpParam2, &LavaProc::lava_wrap_qless}},
    {0x81, op_info_t{"getchar", OpParam0, &LavaProc::lava_wrap_getchar}},
    {0x84, op_info_t{"strlen", OpParam0, &LavaProc::lava_wrap_strlen}},
    {0x87, op_info_t{"delay", OpParam0, &LavaProc::lava_wrap_delay}},
    {0x88, op_info_t{"writeblock", OpParam0, &LavaProc::lava_wrap_writeblock}},
    {0x89, op_info_t{"scroll", OpParam0, &LavaProc::lava_wrap_scroll}},
    {0x8a, op_info_t{"textout", OpParam0, &LavaProc::lava_wrap_textout}},
    {0x8b, op_info_t{"block", OpParam0, &LavaProc::lava_wrap_block}},
    {0x8c, op_info_t{"rectangle", OpParam0, &LavaProc::lava_wrap_rectangle}},
    {0x8d, op_info_t{"exit", OpParam0, &LavaProc::lava_wrap_exit}},
    {0x8e, op_info_t{"clearscreen", OpParam0, &LavaProc::lava_wrap_clearscreen}},
    {0xae, op_info_t{"fopen", OpParam0, &LavaProc::lava_wrap_fopen}},
    {0xaf, op_info_t{"fclose", OpParam0, &LavaProc::lava_wrap_fclose}},
    {0xb0, op_info_t{"fread", OpParam0, &LavaProc::lava_wrap_fread}},
    {0xb1, op_info_t{"fwrite", OpParam0, &LavaProc::lava_wrap_fwrite}},
    {0xb8, op_info_t{"sprintf", OpParam0, &LavaProc::lava_wrap_sprintf}},
    {0xbc, op_info_t{"checkkey", OpParam0, &LavaProc::lava_wrap_checkkey}},
    {0xcb, op_info_t{"setgraphmode", OpParam0, &LavaProc::lava_wrap_setgraphmode}},
    {0xcc, op_info_t{"setbgcolor", OpParam0, &LavaProc::lava_wrap_setbgcolor}},
    {0xcd, op_info_t{"setfgcolor", OpParam0, &LavaProc::lava_wrap_setfgcolor}},
};
