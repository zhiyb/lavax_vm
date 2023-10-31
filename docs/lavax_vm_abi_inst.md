# Instructions

## Set 0

| Value | Name | Size | Description |
|---|---|---|---|
| 0x00 | sn_err | 1 | ? |
| 0x01 | push_char | 2 | ? |
| 0x02 | push_int | 1 | ? |
| 0x03 | push_long | 1 | ? |
| 0x04 | push_vchar | 1 | ? |
| 0x05 | push_vint | 1 | ? |
| 0x06 | push_vlong | 1 | ? |
| 0x07 | push_gchar | 1 | ? |
| 0x08 | push_gint | 1 | ? |
| 0x09 | push_glong | 1 | ? |
| 0x0a | push_achar | 1 | ? |
| 0x0b | push_aint | 1 | ? |
| 0x0c | push_along | 1 | ? |
| 0x0d | push_string | 1 | ? |
| 0x0e | push_lvchar | 1 | ? |
| 0x0f | push_lvint | 1 | ? |
| 0x10 | push_lvlong | 1 | ? |
| 0x11 | push_lgchar | 1 | ? |
| 0x12 | push_lgint | 1 | ? |
| 0x13 | push_lglong | 1 | ? |
| 0x14 | push_lachar | 1 | ? |
| 0x15 | push_laint | 1 | ? |
| 0x16 | push_lalong | 1 | ? |
| 0x17 | push_along | 1 | ? |
| 0x18 | push_lalong | 1 | ? |
| 0x19 | push_llong | 1 | ? |
| 0x1a | push_text | 1 | ? |
| 0x1b | push_graph | 1 | ? |
| 0x1c | push_sub0 | 1 | ? |
| 0x13 | cal_inc | 1 | ? |
| 0x1e | cal_dec | 1 | ? |
| 0x1f | cal_INC | 1 | ? |
| 0x20 | cal_DEC | 1 | ? |
| 0x21 | cal_add | 1 | ? |
| 0x22 | cal_sub | 1 | ? |
| 0x23 | cal_and | 1 | ? |
| 0x24 | cal_or | 1 | ? |
| 0x25 | push_not | 1 | ? |
| 0x26 | cal_xor | 1 | ? |
| 0x27 | cal_land | 1 | ? |
| 0x28 | cal_lor | 1 | ? |
| 0x29 | cal_lnot | 1 | ? |
| 0x2a | cal_mul | 1 | ? |
| 0x2b | cal_div | 1 | ? |
| 0x2c | cal_mod | 1 | ? |
| 0x2d | cal_lshift | 1 | ? |
| 0x2e | cal_rshift | 1 | ? |
| 0x2f | cal_equ | 1 | ? |
| 0x30 | cal_neq | 1 | ? |
| 0x31 | cal_le | 1 | ? |
| 0x32 | cal_ge | 1 | ? |
| 0x33 | cal_great | 1 | ? |
| 0x34 | cal_less | 1 | ? |
| 0x35 | c_let | 1 | ? |
| 0x36 | c_ptr | 1 | ? |
| 0x37 | c_cptr | 1 | ? |
| 0x38 | pop_val | 1 | ? |
| 0x39 | c_jmpe | 1 | ? |
| 0x3a | c_jmpn | 1 | ? |
| 0x3b | c_jmp | 1 | ? |
| 0x3c | set_sp | 1 | ? |
| 0x3d | c_call | 1 | ? |
| 0x3e | add_bp | 1 | ? |
| 0x3f | sub_bp | 1 | ? |
| 0x40 | good_exit | 1 | ? |
| 0x41 | c_preset | 1 | ? |
| 0x42 | push_gbuf | 1 | ? |
| 0x43 | c_secret | 1 | ? |
| 0x44 | c_loadall | 1 | ? |
| 0x45 | cal_qadd | 1 | ? |
| 0x46 | cal_qsub | 1 | ? |
| 0x47 | cal_qmul | 1 | ? |
| 0x48 | cal_qdiv | 1 | ? |
| 0x49 | cal_qmod | 1 | ? |
| 0x4a | cal_qlshift | 1 | ? |
| 0x4b | cal_qrshift | 1 | ? |
| 0x4c | cal_qequ | 1 | ? |
| 0x4d | cal_qneq | 1 | ? |
| 0x4e | cal_qgreat | 1 | ? |
| 0x4f | cal_qless | 1 | ? |
| 0x50 | cal_qge | 1 | ? |
| 0x51 | cal_qle | 1 | ? |
| 0x52 | c_iptr | 1 | ? |
| 0x53 | c_lptr | 1 | ? |
| 0x54 | c_icf | 1 | ? |
| 0x55 | c_fci | 1 | ? |
| 0x56 | cal_addff | 1 | ? |
| 0x57 | cal_addf | 1 | ? |
| 0x58 | cal_add0f | 1 | ? |
| 0x59 | cal_subff | 1 | ? |
| 0x5a | cal_subf | 1 | ? |
| 0x5b | cal_sub0f | 1 | ? |
| 0x5c | cal_mulff | 1 | ? |
| 0x5d | cal_mulf | 1 | ? |
| 0x5e | cal_mul0f | 1 | ? |
| 0x5f | cal_divff | 1 | ? |
| 0x60 | cal_divf | 1 | ? |
| 0x61 | cal_div0f | 1 | ? |
| 0x62 | push_sub0f | 1 | ? |
| 0x63 | cal_lessf | 1 | ? |
| 0x64 | cal_greatf | 1 | ? |
| 0x65 | cal_equf | 1 | ? |
| 0x66 | cal_neqf | 1 | ? |
| 0x67 | cal_lef | 1 | ? |
| 0x68 | cal_gef | 1 | ? |
| 0x69 | c_f0 | 1 | ? |
| 0x6a | c_ciptr | 1 | ? |
| 0x6b | c_clptr | 1 | ? |
| 0x6c | c_lcc | 1 | ? |
| 0x6d | c_lci | 1 | ? |
| 0x6e | c_letx | 1 | ? |
| 0x6f | push_ax | 1 | ? |
| 0x70 | cal_idx | 1 | ? |
| 0x71 | c_pass | 1 | ? |
| 0x72 | c_void | 1 | ? |
| 0x73 | c_debug | 1 | ? |
| 0x74 | c_funcid | 1 | ? |

## Set 1

| Value | Name | Size | Description |
|---|---|---|---|
| 0x80 | c_putchar | 1 | ? |
| 0x81 | c_getchar | 1 | ? |
| 0x82 | c_printf | 1 | ? |
| 0x83 | c_strcpy | 1 | ? |
| 0x84 | c_strlen | 1 | ? |
| 0x85 | c_setscreen | 1 | ? |
| 0x86 | c_updatelcd | 1 | ? |
| 0x87 | c_delay | 1 | ? |
| 0x88 | c_writeblock | 1 | ? |
| 0x89 | scroll_to_lcd | 1 | ? |
| 0x8a | c_textout | 1 | ? |
| 0x8b | c_block | 1 | ? |
| 0x8c | c_rectangle | 1 | ? |
| 0x8d | c_exit | 1 | ? |
| 0x8e | c_clearscreen | 1 | ? |
| 0x8f | c_abs | 1 | ? |
| 0x90 | c_rand | 1 | ? |
| 0x91 | c_srand | 1 | ? |
| 0x92 | c_locate | 1 | ? |
| 0x93 | c_inkey | 1 | ? |
| 0x94 | c_point | 1 | ? |
| 0x95 | c_getpoint | 1 | ? |
| 0x96 | c_line | 1 | ? |
| 0x97 | c_box | 1 | ? |
| 0x98 | c_circle | 1 | ? |
| 0x99 | c_ellipse | 1 | ? |
| 0x9a | c_beep | 1 | ? |
| 0x9b | c_isalnum | 1 | ? |
| 0x9c | c_isalpha | 1 | ? |
| 0x9d | c_iscntrl | 1 | ? |
| 0x9e | c_isdigit | 1 | ? |
| 0x9f | c_isgraph | 1 | ? |
| 0xa0 | c_islower | 1 | ? |
| 0xa1 | c_isprint | 1 | ? |
| 0xa2 | c_ispunct | 1 | ? |
| 0xa3 | c_isspace | 1 | ? |
| 0xa4 | c_isupper | 1 | ? |
| 0xa5 | c_isxdigit | 1 | ? |
| 0xa6 | c_strcat | 1 | ? |
| 0xa7 | c_strchr | 1 | ? |
| 0xa8 | c_strcmp | 1 | ? |
| 0xa9 | c_strstr | 1 | ? |
| 0xaa | c_tolower | 1 | ? |
| 0xab | c_toupper | 1 | ? |
| 0xac | c_memset | 1 | ? |
| 0xad | c_memcpy | 1 | ? |
| 0xae | c_fopen | 1 | ? |
| 0xaf | c_fclose | 1 | ? |
| 0xb0 | c_fread | 1 | ? |
| 0xb1 | c_fwrite | 1 | ? |
| 0xb2 | c_fseek | 1 | ? |
| 0xb3 | c_ftell | 1 | ? |
| 0xb4 | c_feof | 1 | ? |
| 0xb5 | c_rewind | 1 | ? |
| 0xb6 | c_getc | 1 | ? |
| 0xb7 | c_putc | 1 | ? |
| 0xb8 | c_sprintf | 1 | ? |
| 0xb9 | c_makedir | 1 | ? |
| 0xba | c_deletefile | 1 | ? |
| 0xbb | c_getms | 1 | ? |
| 0xbc | c_checkkey | 1 | ? |
| 0xbd | c_memmove | 1 | ? |
| 0xbe | c_crc16 | 1 | ? |
| 0xbf | c_jiami | 1 | ? |
| 0xc0 | c_chdir | 1 | ? |
| 0xc1 | c_filelist | 1 | ? |
| 0xc2 | c_gettime | 1 | ? |
| 0xc3 | c_settime | 1 | ? |
| 0xc4 | c_getword | 1 | ? |
| 0xc5 | c_xdraw | 1 | ? |
| 0xc6 | c_releasekey | 1 | ? |
| 0xc7 | c_getblock | 1 | ? |
| 0xc8 | c_sin | 1 | ? |
| 0xc9 | c_cos | 1 | ? |
| 0xca | c_fill | 1 | ? |
| 0xcb | c_setgraphmode | 1 | ? |
| 0xcc | c_setbgcolor | 1 | ? |
| 0xcd | c_setfgcolor | 1 | ? |
| 0xce | c_setlist | 1 | ? |
| 0xcf | c_fade | 1 | ? |
| 0xd0 | c_exec | 1 | ? |
| 0xd1 | c_findfile | 1 | ? |
| 0xd2 | c_getfilenum | 1 | ? |
| 0xd3 | c_system | 1 | ? |
| 0xd4 | c_math | 1 | ? |
| 0xd5 | c_setpalette | 1 | ? |
| 0xd6 | c_getcmdline | 1 | ? |

If bit 7 == 0, continue without delay for 1000 times, for debugging?

## push_char

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x01 |
| +1 | u8 | dp0 | Data |

Stack push:

| Type | Name | Description |
|---|---|---|
| u8 | dp0 | Data |

Push `u8` type `dp0` onto stack.

## push_int

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x02 |
| +2 | i16 | dp0 | Data |

Stack push:

| Type | Name | Description |
|---|---|---|
| i16 | dp0 | Data |

Push `i16` type `dp0` onto stack.

## push_long

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x03 |
| +4 | i32 | dp0 | Data |

Stack push:

| Type | Name | Description |
|---|---|---|
| i32 | dp0 | Data |

Push `i32` type `dp0` onto stack.

## push_vchar

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x04 |
| +2/+3 | addr | ptr0 | RAM pointer |

Stack push:

| Type | Name | Description |
|---|---|---|
| u8 | ds0 | Data |

Push `u8` type `ds0` at `*ptr0` onto stack.

## push_vint

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x05 |
| +2/+3 | addr | ptr0 | RAM pointer |

Stack push:

| Type | Name | Description |
|---|---|---|
| i16 | ds0 | Data |

Push `i16` type `ds0` at `*ptr0` onto stack.

## push_vlong

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x06 |
| +2/+3 | addr | ptr0 | RAM pointer |

Stack push:

| Type | Name | Description |
|---|---|---|
| i32 | ds0 | Data |

Push `i32` type `ds0` at `*ptr0` onto stack.

## push_gchar

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x07 |
| +2/+3 | addr | ptr0 | RAM pointer |

Stack pop:

| Type | Name | Description |
|---|---|---|
| addr | offset | Address offset |

Stack push:

| Type | Name | Description |
|---|---|---|
| u8 | ds0 | Data |

Push `u8` type `ds0` at `*(ptr0 + offset)` onto stack.

## push_gint

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x08 |
| +2/+3 | addr | ptr0 | RAM pointer |

Stack pop:

| Type | Name | Description |
|---|---|---|
| addr | offset | Address offset |

Stack push:

| Type | Name | Description |
|---|---|---|
| i16 | ds0 | Data |

Push `i16` type `ds0` at `*(ptr0 + offset)` onto stack.

## push_glong

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x09 |
| +2/+3 | addr | ptr0 | RAM pointer |

Stack pop:

| Type | Name | Description |
|---|---|---|
| addr | offset | Address offset |

Stack push:

| Type | Name | Description |
|---|---|---|
| i32 | ds0 | Data |

Push `i32` type `ds0` at `*(ptr0 + offset)` onto stack.

## push_achar

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x0a |
| +2/+3 | addr | ptr0 | RAM pointer |

Stack pop:

| Type | Name | Description |
|---|---|---|
| addr | offset | Address offset |

Stack push:

| Type | Name | Description |
|---|---|---|
| addr_u8 | v_addr | Data |

Push size-included address `v_addr` with value `ptr0 + offset` onto stack.\
For `*u8` type address, set higher byte to 1.

```python
if RamBits <= 16:
	v_addr = 0x00010000 | (ptr0 + offset)
else:
	v_addr = 0x01000000 | (ptr0 + offset)
```

## push_string

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x0d |
| variable | string | str0 | 0-terminated string data |

Stack push:

| Type | Name | Description |
|---|---|---|
| u32 | sptr | Current string pointer |

String stack push:

| Type | Name | Description |
|---|---|---|
| string | str0 | 0-terminated string data |

Push current string pointer onto stack.

Decode string data using one-byte XOR secret (see `c_secret`).

Push decoded string data onto string stack, including terminator 0.

## push_along

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x17 |
| +2/+3 | addr | ptr0 | RAM pointer |

Stack pop:

| Type | Name | Description |
|---|---|---|
| addr | offset | Address offset |

Stack push:

| Type | Name | Description |
|---|---|---|
| addr_i32 | v_addr | Data |

Push size-included address `v_addr` with value `ptr0 + offset` onto stack.\
For `*i32` type address, set higher byte to 0.

```python
v_addr = ptr0 + offset
```

## cal_INC

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x1f |

Stack pop:

| Type | Name | Description |
|---|---|---|
| i32 | ds0 | Stack data 0 (a3) |

```c
void inc_dec_com()
{
	long t;
	a3 = ds0;

	if (RamBits>16) {
		t=(a3>>24)&0x7f;
		if (a3&0x80000000) a3+=local_bp; //???有可能越界，但也可以不检验
		a3&=0xffffff;
		if (t==1) a1=lRamRead(a3);
		else if (t==2) a1=lRamRead2(a3);
		else a1=lRamRead4(a3);
		a3|=t<<24;
	} else {
		t=(a3>>16)&0x7f;
		if (a3&0x800000) a3+=local_bp; //???有可能越界，但也可以不检验
		a3&=0xffff;
		if (t==1) a1=lRamRead(a3);
		else if (t==2) a1=lRamRead2(a3);
		else a1=lRamRead4(a3);
		a3|=t<<16;
	}
}

void put_val()
{
	*(long *)(lRam+eval_stack+eval_top)=a1;
	eval_top+=4;
}

void inc_save()
{
	int t;
	if (RamBits>16) {
		t=(a3>>24)&0x7f;
		a3&=0xffffff;
	} else {
		t=(a3>>16)&0x7f;
		a3&=0xffff;
	}
	if (t==1) lRamWrite(a3,(byte)(a1&0xff));
	else if (t==2) lRamWrite2(a3,(short)(a1&0xffff));
	else lRamWrite4(a3,a1);
}

void cal_inc()
{
	inc_dec_com();
	a1++;
	put_val();
	inc_save();
}
```

## cal_DEC

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x20 |

```c
void get_val2()
{
	eval_top-=4;
	a3=*(long *)(lRam+eval_stack+eval_top);
}

void inc_dec_com()
{
	long t;
	get_val2();
	if (RamBits>16) {
		t=(a3>>24)&0x7f;
		if (a3&0x80000000) a3+=local_bp; //???有可能越界，但也可以不检验
		a3&=0xffffff;
		if (t==1) a1=lRamRead(a3);
		else if (t==2) a1=lRamRead2(a3);
		else a1=lRamRead4(a3);
		a3|=t<<24;
	} else {
		t=(a3>>16)&0x7f;
		if (a3&0x800000) a3+=local_bp; //???有可能越界，但也可以不检验
		a3&=0xffff;
		if (t==1) a1=lRamRead(a3);
		else if (t==2) a1=lRamRead2(a3);
		else a1=lRamRead4(a3);
		a3|=t<<16;
	}
}

void put_val()
{
	*(long *)(lRam+eval_stack+eval_top)=a1;
	eval_top+=4;
}

void inc_save()
{
	int t;
	if (RamBits>16) {
		t=(a3>>24)&0x7f;
		a3&=0xffffff;
	} else {
		t=(a3>>16)&0x7f;
		a3&=0xffff;
	}
	if (t==1) lRamWrite(a3,(byte)(a1&0xff));
	else if (t==2) lRamWrite2(a3,(short)(a1&0xffff));
	else lRamWrite4(a3,a1);
}

void cal_DEC()
{
	inc_dec_com();
	put_val();
	a1--;
	inc_save();
}
```

## cal_add

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x21 |

Stack pull:

| Type | Name | Description |
|---|---|---|
| i32 | ds0 | Stack data 0 (a3) |
| i32 | ds1 | Stack data 1 (a1) |

Stack push:

| Type | Name | Description |
|---|---|---|
| i32 | ret | Return value |

```c
ret = ds1 + ds0
```

## cal_sub

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x22 |

Stack pull:

| Type | Name | Description |
|---|---|---|
| i32 | ds0 | Stack data 0 (a3) |
| i32 | ds1 | Stack data 1 (a1) |

Stack push:

| Type | Name | Description |
|---|---|---|
| i32 | ret | Return value |

```python
ret = ds1 - ds0
```

## cal_land

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x27 |

Stack pull:

| Type | Name | Description |
|---|---|---|
| i32 | ds0 | Stack data 0 (a3) |
| i32 | ds1 | Stack data 1 (a1) |

Stack push:

| Type | Name | Description |
|---|---|---|
| i32 | ret | Return value |

```python
if ds0 and ds1:
    ret = LTRUE
else:
    ret = LFALSE
```

## cal_lor

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x28 |

Stack pop:

| Type | Name | Description |
|---|---|---|
| i32 | ds0 | Stack data 0 |
| i32 | ds1 | Stack data 1 |

Stack push:

| Type | Name | Description |
|---|---|---|
| u32 | ret | Return value |

```python
if ds0 or ds1:
    ret = LTRUE
else:
    ret = LFALSE
```

## cal_mul

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x2a |

Stack pop:

| Type | Name | Description |
|---|---|---|
| i32 | ds0 | Stack data 0 (a3) |
| i32 | ds1 | Stack data 1 (a1) |

Stack push:

| Type | Name | Description |
|---|---|---|
| u32 | ret | Return value |

```python
ret = ds0 * ds1
```

## cal_div

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x2b |

Stack pop:

| Type | Name | Description |
|---|---|---|
| i32 | ds0 | Stack data 0 (a3) |
| i32 | ds1 | Stack data 1 (a1) |

Stack push:

| Type | Name | Description |
|---|---|---|
| u32 | ret | Return value |

```python
if ds0 == 0:
    div0_err
else:
    ret = ds1 / ds0
```

## cal_equ

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x2a |

Stack pop:

| Type | Name | Description |
|---|---|---|
| i32 | ds0 | Stack data 0 (a3) |
| i32 | ds1 | Stack data 1 (a1) |

Stack push:

| Type | Name | Description |
|---|---|---|
| u32 | ret | Return value |

```python
if ds0 == ds1:
    ret = LTRUE
else:
    ret = LFALSE
```

## cal_le

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x31 |

Stack pop:

| Type | Name | Description |
|---|---|---|
| i32 | ds0 | Stack data 0 (a3) |
| i32 | ds1 | Stack data 1 (a1) |

Stack push:

| Type | Name | Description |
|---|---|---|
| u32 | ret | Return value |

```python
if ds1 <= ds0:
    ret = LTRUE
else:
    ret = LFALSE
```

## cal_ge

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x32 |

Stack pop:

| Type | Name | Description |
|---|---|---|
| i32 | ds0 | Stack data 0 (a3) |
| i32 | ds1 | Stack data 1 (a1) |

Stack push:

| Type | Name | Description |
|---|---|---|
| u32 | ret | Return value |

```python
if ds1 >= ds0:
    ret = LTRUE
else:
    ret = LFALSE
```

## cal_great

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x33 |

Stack pop:

| Type | Name | Description |
|---|---|---|
| i32 | ds0 | Stack data 0 (a3) |
| i32 | ds1 | Stack data 1 (a1) |

Stack push:

| Type | Name | Description |
|---|---|---|
| u32 | ret | Return value |

```python
if ds1 > ds0:
    ret = LTRUE
else:
    ret = LFALSE
```

## cal_less

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x34 |

Stack pop:

| Type | Name | Description |
|---|---|---|
| i32 | ds0 | Stack data 0 (a3) |
| i32 | ds1 | Stack data 1 (a1) |

Stack push:

| Type | Name | Description |
|---|---|---|
| u32 | ret | Return value |

```python
if ds1 < ds0:
    ret = LTRUE
else:
    ret = LFALSE
```

## c_let

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x35 |

Stack peek:

| Type | Name | Description |
|---|---|---|
| u32 | data0 | a3 |
| u32 | data1 | a1 |

TODO WHAT IS THIS?

```c

void c_let()
{
	long t,t1;
	get_vals();
	if (RamBits>16) {
		if (a1&0x80000000) t1=(a1+local_bp)&0xffffff; //???有可能越界，但也可以不检验
		else t1=a1&0xffffff;
		t=(a1>>24)&0x7f;
	} else {
		if (a1&0x800000) t1=(a1+local_bp)&0xffff; //???有可能越界，但也可以不检验
		else t1=a1&0xffff;
		t=(a1>>16)&0x7f;
	}
	if (t==1) lRamWrite(t1,(byte)(a3&0xff));
	else if (t==2) lRamWrite2(t1,(short)(a3&0xffff));
	else lRamWrite4(t1,a3);
	a1=a3;
	put_val();
}

```

## pop_val

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x38 |

Stack pop:

| Type | Name | Description |
|---|---|---|
| i32 | res | Result value |

Pop `i32` type data from stack to `result` flag.\
See `jmpe`, `jmpn`.

## c_jmpe

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x39 |
| +3 | u24 | addr0 | PC address |

If `result` flag is 0, jump to PC address `addr0`.

## c_jmp

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x3b |
| +3 | u24 | addr0 | PC address |

If `result` flag is not 0, jump to PC address `addr0`.

## set_sp

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x3c |
| +2/+3 | addr | addr0 | Stack pointer address |

Set stack pointer to `addr0`.

## add_bp

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x3e |
| +2/+3 | addr | offset | Address offset |
| +1 | u8 | count | Data |

TODO WHAT IS THIS

```c

void add_bp()
{
	i32 t = local_bp;
	local_bp = local_sp;

	lRamWrite(local_bp+3,(byte)(t&0xff));
	lRamWrite(local_bp+4,(byte)((t>>8)&0xff));
	if (RamBits>16) lRamWrite(local_bp+5,(byte)((t>>16)&0xff));

	! local_sp = local_sp + offset;
	! t = count * 4;

	if (t) {
		eval_top -= t;
		u8 t2 = eval_top;
		int i = 0;
		if (RamBits==32) {
			while (t) {
				lRamWrite(local_bp+8+i++,lRam[eval_stack+t2++]);
				t--;
			}
		} else if (RamBits>16) {
			while (t) {
				lRamWrite(local_bp+6+i++,lRam[eval_stack+t2++]);
				t--;
			}
		} else {
			while (t) {
				lRamWrite(local_bp+5+i++,lRam[eval_stack+t2++]);
				t--;
			}
		}
	}
}

```

## good_exit

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x40 |

Exit program with exit code 0.
See `c_exit`.

## cal_qadd

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x45 |
| +2 | i16 | dp0 | Parameter data 0 |

Stack pop:

| Type | Name | Description |
|---|---|---|
| i32 | ds0 | Stack data 0 |

Stack push:

| Type | Name | Description |
|---|---|---|
| u32 | ret | Return value |

```python
ret = ds0 + dp0
```

## cal_qsub

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x46 |
| +2 | i16 | dp0 | Parameter data 0 (a3) |

Stack pop:

| Type | Name | Description |
|---|---|---|
| i32 | ds0 | Stack data 0 (a1) |

Stack push:

| Type | Name | Description |
|---|---|---|
| u32 | ret | Return value |

```python
ret = ds0 - dp0
```

## cal_qmul

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x47 |
| +2 | i16 | dp0 | Parameter data 0 |

Stack pop:

| Type | Name | Description |
|---|---|---|
| i32 | ds0 | Stack data 0 |

Stack push:

| Type | Name | Description |
|---|---|---|
| u32 | ret | Return value |

```python
ret = ds0 * dp0
```

## cal_qdiv

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x48 |
| +2 | i16 | dp0 | Parameter data 0 (a3) |

Stack pop:

| Type | Name | Description |
|---|---|---|
| i32 | ds0 | Stack data 0 (a1) |

Stack push:

| Type | Name | Description |
|---|---|---|
| u32 | ret | Return value |

```python
if dp0 == 0:
    div0_err
else
    ret = ds0 / dp0
```

## cal_qlshift

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x4a |
| +2 | i16 | dp0 | Parameter data 0 (a3) |

Stack pop:

| Type | Name | Description |
|---|---|---|
| i32 | ds0 | Stack data 0 (a1) |

Stack push:

| Type | Name | Description |
|---|---|---|
| i32 | ret | Return value |

```python
ret = ds0 << dp0
```

## cal_qequ

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x4c |
| +2 | i16 | dp0 | Parameter data 0 |

Stack pop:

| Type | Name | Description |
|---|---|---|
| i32 | ds0 | Stack data 0 |

Stack push:

| Type | Name | Description |
|---|---|---|
| u32 | ret | Return value |

```python
if ds0 == dp0:
    ret = LTRUE
else:
    ret = LFALSE
```

## cal_qneq

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x4d |
| +2 | i16 | dp0 | Parameter data 0 |

Stack pop:

| Type | Name | Description |
|---|---|---|
| i32 | ds0 | Stack data 0 |

Stack push:

| Type | Name | Description |
|---|---|---|
| u32 | ret | Return value |

```python
if ds0 != dp0:
    ret = LTRUE
else:
    ret = LFALSE
```

## cal_qgreat

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x4e |
| +2 | i16 | dp0 | Parameter data 0 (a3) |

Stack pop:

| Type | Name | Description |
|---|---|---|
| i32 | ds0 | Stack data 0 (a1) |

Stack push:

| Type | Name | Description |
|---|---|---|
| u32 | ret | Return value |

```python
if ds0 > dp0:
    ret = LTRUE
else:
    ret = LFALSE
```

## cal_qless

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x4f |
| +2 | i16 | dp0 | Parameter data 0 |

Stack pop:

| Type | Name | Description |
|---|---|---|
| i32 | ds0 | Stack data 0 |

Stack push:

| Type | Name | Description |
|---|---|---|
| u32 | ret | Return value |

```python
if (ds0 < dp0)
    ret = LTRUE
else
    ret = LFALSE
```

## c_getchar

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x81 |

```c
void c_getchar()
{
	wait_key=1;
	if (have_pen) pen_x=-1;
}
```

## c_strlen

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x84 |

Stack pop:

| Type | Name | Description |
|---|---|---|
| addr | sptr | String pointer |

Stack push:

| Type | Name | Description |
|---|---|---|
| i32 | ret | Return data |

Calculate 0-terminated string length at `*sptr`.\
Push string length excluding 0-terminator to `ret`.

## c_delay

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x87 |

Stack pop:

| Type | Name | Description |
|---|---|---|
| u16 | delay | Delay time |

Delay for `delay` ms, maximum `0x7fff (32767)` ms.

## c_writeblock

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x88 |

```c
void c_writeblock()
{
	a32 data;
	word x,y,width,height;
	get_val();
	if (RamBits>16) data=a1&0xffffff;
	else data=(word)a1;
	get_val();
	no_buf=a1&0x40;
	negative=a1&0x20;
	lcmd=a1&0xf;
	get_val();
	height=(word)a1;
	get_val();
	width=(word)a1;
	get_val();
	y=(word)a1;
	get_val();
	x=(word)a1;
	if (width && height) {
		if (graph_mode==1)
			CheckAddr(data,data+((width+7)/8)*height-1,4);
		else
			CheckAddr(data,data+((width+1)/2)*height-1,4);
	}
	write_comm(x,y,width,height,lRam+data);
}
```

## scroll_to_lcd

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x89 |

```c
void scroll_to_lcd()
{
    memcpy(BmpData,BmpData+SCROLL_CON,LCD_WIDTH*LCD_HEIGHT);
}
```

## c_textout

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x8a |

```c
void c_textout()
{
	byte t,c;
	word font_x,font_y;
	a32 addr;
	get_val();
	no_buf=a1&0x40;
	negative=a1&0x20;
	lcmd=a1&0xf;
	t=(byte)a1;
	get_val();
	if (RamBits>16) addr=a1&0xffffff;
	else addr=(word)a1;
	get_val();
	font_y=(word)a1;
	get_val();
	font_x=(word)a1;
	if ((t&0x80)==0) {
		for (;;) {
			if (font_x>=ScreenWidth) break;
			c=lRamRead(addr++);
			if (c==0) break;
			if (c<128) {
				font_6x12(font_x,font_y,c);
				font_x+=6;
			} else {
				t=lRamRead(addr++);
				font_12x12(font_x,font_y,c,t);
				font_x+=12;
			}
		}
	} else {
		for (;;) {
			if (font_x>=ScreenWidth) break;
			c=lRamRead(addr++);
			if (c==0) break;
			if (c<128) {
				font_8x16(font_x,font_y,c);
				font_x+=8;
			} else {
				t=lRamRead(addr++);
				font_16x16(font_x,font_y,c,t);
				font_x+=16;
			}
		}
	}
}
```

## c_block

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x8b |

```c

void block_comm()
{
	get_val();
	no_buf=a1&0x40;
	lcmd=a1&3;
	get_val();
	Y1=(word)a1;
	get_val();
	x1=(word)a1;
	get_val();
	Y0=(word)a1;
	get_val();
	x0=(word)a1;
}

void block_check()
{
	word t;
	if (Y0>Y1) {
		t=Y0;
		Y0=Y1;
		Y1=t;
	}
	if (x0>x1) {
		t=x0;
		x0=x1;
		x1=t;
	}
	if (x0>=ScreenWidth) x0=ScreenWidth-1;
	if (x1>=ScreenWidth) x1=ScreenWidth-1;
	if (Y0>=ScreenHeight) Y0=ScreenHeight-1;
	if (Y1>=ScreenHeight) Y1=ScreenHeight-1;
}

void ByteAddr()
{
	m1l=yy*LCD_WIDTH+xx;
	if (!no_buf) m1l+=SCROLL_CON;
}

void hline()
//调用前必须调整Y0,x0,x1使其满足：
//Y0<ScreenHeight x0<ScreenWidth x1<ScreenWidth x0<=x1
{
	word width;
	byte *p;
	yy=Y0;
	xx=x0;
	ByteAddr();
	width=x1-x0+1;
	p=BmpData+m1l;
	//for (xx=x0;xx<=x1;xx++) put_dot();
	if (graph_mode==1) {
		switch (lcmd) {
		case 0:
			memset(p,0,width);
			break;
		case 1:
			memset(p,1,width);
			break;
		case 2:
			while (width--) {
				*p^=1;
				p++;
			}
			break;
		}
	} else if (graph_mode==4) {
		switch (lcmd) {
		case 0:
			memset(p,(byte)bgcolor,width);
			break;
		case 1:
			memset(p,(byte)fgcolor,width);
			break;
		case 2:
			while (width--) {
				*p^=15;
				p++;
			}
			break;
		}
	} else {
		switch (lcmd) {
		case 0:
			memset(p,(byte)bgcolor,width);
			break;
		case 1:
			memset(p,(byte)fgcolor,width);
			break;
		case 2:
			while (width--) {
				*p^=255;
				p++;
			}
			break;
		}
	}
}

void block_draw()
{
	word t;
	block_check();
	t=Y1-Y0+1;
	while (t) {
		hline();
		Y0++;
		t--;
	}
}

void c_block()
{
	block_comm();
	block_draw();
}

```

## c_rectangle

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x8c |

```c
void square_draw()
{
	word t;
	block_check();
	hline();
	vline();
	t=x0;
	x0=x1;
	vline();
	x0=t;
	Y0=Y1;
	hline();
}

void c_rectangle()
{
	block_comm();
	square_draw();
}
```

## c_exit

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x8d |

Stack pop:

| Type | Name | Description |
|---|---|---|
| i32 | retval | Exit code |

Exit program with `retval` exit code.

## c_clearscreen

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x8e |

Clears the entire screen.

## c_fopen

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0xae |

Stack pop:

| Type | Name | Description |
|---|---|---|
| u32 | sptr_mode | String pointer to file mode |
| u32 | sptr_fpath | String pointer to file path |

Stack push:

| Type | Name | Description |
|---|---|---|
| u8 | fd | File descriptor |

Open file for read/write.

Allowed file mode strings:\
`{"r","rb","r+","rb+","w","wb","w+","wb+","a","ab","a+","ab+"}`

Return 0 if failed, else file descriptor (OR'ed with `0x80`).

## c_fclose

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0xaf |

Stack pop:

| Type | Name | Description |
|---|---|---|
| u8 | fd | File descriptor |

Close file descriptor.

## c_fread

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0xb0 |

```c
void c_fread()
{
	a32 len,str;
	if (!check_handle()) {
		eval_top-=12;
		a1=0;
		put_val();
		return;
	}
	get_val();
	if (RamBits>16) len=a1&0xffffff;
	else len=(word)a1;
	get_val();
	get_val();
	if (RamBits>16) str=a1&0xffffff;
	else str=(word)a1;
	if (modeAccess[fmode[curr_fnum]]&GENERIC_READ) {
		if (foffset[curr_fnum]+len>fsize[curr_fnum])
			len=fsize[curr_fnum]-foffset[curr_fnum]; //读文件不超过文件尾
		if (len) {
			CheckAddr(str,str+len-1,6);
			if (!ReadFile(FileHandle,lRam+str,len,&a1,NULL)) a1=0;
			foffset[curr_fnum]+=a1;
		} else a1=0;
	} else a1=0;
	put_val();
}
```

## c_fwrite

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0xb1 |

```c
void c_fwrite()
{
	a32 len,str;
	if (!check_handle()) {
		eval_top-=12;
		a1=0;
		put_val();
		return;
	}
	get_val();
	if (RamBits>16) len=a1&0xffffff;
	else len=(word)a1;
	get_val();
	get_val();
	if (RamBits>16) str=a1&0xffffff;
	else str=(word)a1;
	if (modeAccess[fmode[curr_fnum]]&GENERIC_WRITE) {
		if (len) {
			CheckAddr(str,str+len-1,7);
			if (!WriteFile(FileHandle,lRam+str,len,&a1,NULL)) a1=0;
			foffset[curr_fnum]+=a1;
			if (foffset[curr_fnum]>fsize[curr_fnum])
			fsize[curr_fnum]=foffset[curr_fnum];
		} else a1=0;
	} else a1=0;
	put_val();
}
```

## c_sprintf

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0xb8 |

```c
void c_sprintf()
{
	a32 str1,fmt_str,str2;
	byte c;
	char num[20];
	int i,digit,flag,real_digit;

	get_val();
	eval_top-=(byte)(a1<<2);
	func_x=eval_top;
	if (RamBits>16) str1=*(long *)(lRam+eval_stack+eval_top)&0xffffff;
	else str1=*(word *)(lRam+eval_stack+eval_top);
	next_arg();
	if (RamBits>16) fmt_str=a1&0xffffff;
	else fmt_str=(word)a1;
	for (;;) {
		c=lRamRead(fmt_str++);
		if (c==0) break;
		if (c=='%') {
			fmt_str=fenxi_fmt(fmt_str,&digit,&flag);
			c=lRamRead(fmt_str++);
			if (c==0) break;
			if (c=='d') {
				next_arg();
				sprintf(num,"%ld",a1);
				real_digit=strlen(num);
				if (digit==0 || real_digit>=digit) {
					for (i=0;;i++) {
						if (num[i]) lRamWrite(str1++,num[i]);
						else break;
					}
				} else {
					if (flag==0x80) {
						for (i=0;;i++) {
							if (num[i]) lRamWrite(str1++,num[i]);
							else break;
						}
						for (i=0;i<digit-real_digit;i++) {
							lRamWrite(str1++,' ');
						}
					} else if (flag==0x40) {
						for (i=0;i<digit-real_digit;i++) {
							lRamWrite(str1++,'0');
						}
						for (i=0;;i++) {
							if (num[i]) lRamWrite(str1++,num[i]);
							else break;
						}
					} else {
						for (i=0;i<digit-real_digit;i++) {
							lRamWrite(str1++,' ');
						}
						for (i=0;;i++) {
							if (num[i]) lRamWrite(str1++,num[i]);
							else break;
						}
					}
				}
			} else if (c=='f') {
				int flag;
				next_arg();
				if (((a1>>23)&0xff)==0xff) {
					strcpy(num,"error");
				} else
					sprintf(num,"%g",i2f(a1));
				flag=0;
				for (i=0;;i++) {
					if (num[i]=='e' && i) flag=1;
					if (flag) flag++;
					if (flag==4) continue; //转换x.xxxxxe+0yy为x.xxxxxe+yy
					if (num[i]) lRamWrite(str1++,num[i]);
					else break;
				}
			} else if (c=='%') lRamWrite(str1++,c);
			else if (c=='c') {
				next_arg();
				lRamWrite(str1++,(byte)a1);
			} else if (c=='s') {
				next_arg();
				if (RamBits>16) str2=a1&0xffffff;
				else str2=(word)a1;
				for (;;) {
					c=lRamRead(str2++);
					if (c==0) break;
					lRamWrite(str1++,c);
				}
			} else lRamWrite(str1++,c);
		} else {
			lRamWrite(str1++,c);
		}
	}
	lRamWrite(str1,0);
}
```

## c_checkkey

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0xbc |

```c
void c_checkkey()
{
	byte k;
	int i;
	get_val();
	k=(byte)a1;
	a1=LFALSE;
	if (k<128) {
		k=c_keyid(k);
		if (k && (cur_keyb[k]&0x80)) a1=LTRUE;
	} else {
		for (i=0;i<sizeof(cur_keyb);i++) {
			if (cur_keyb[i]&0x80) {
				k=c_keyval((byte)i);
				if (k) {
					a1=k;
					break;
				}
			}
		}
	}
	put_val();
}
```

## c_setgraphmode

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0xcb |

Stack pop:

| Type | Name | Description |
|---|---|---|
| u32 | mode | Graphic mode |

Stack push:

| Type | Name | Description |
|---|---|---|
| u32 | ret | Return value |

If mode == 0, return current graphic mode.

If mode == 1/4/8, change graphic mode, see "Graphic mode" secion in configuration byte. Also clears the entire screen.

Else, return 0.

## c_setbgcolor

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0xcc |

Stack pop:

| Type | Name | Description |
|---|---|---|
| u32 | bgcolour | Background colour |

Set foregounrd colour.

In monochromatic (1) or 16-colour (4) mode, set background colour to `bgcolour & 0x0f`.\
In 256-colour (8) mode, set background colour to `bgcolour & 0xff`.

## c_setfgcolor

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0xcd |

Stack pop:

| Type | Name | Description |
|---|---|---|
| u32 | fgcolour | Foreground colour |

Set foregounrd colour.

In monochromatic (1) or 16-colour (4) mode, set foreground colour to `fgcolour & 0x0f`.\
In 256-colour (8) mode, set foreground colour to `fgcolour & 0xff`.
