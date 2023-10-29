# LavaX VM bytecode documentation

This is documentation about the .lav LavaX VM file format.

## File header

First 16-bytes in the file are the special file header.

| Byte index | Example HEX | Description |
|---|---|---|
| 0-3 | 4c 41 56 12 | File signature (LAV?) |
| 4-7 | 00 00 00 00 | Reserved, keep as 0 |
| 8 | 40 | Configuration byte 0 |
| 9 | 14 | Window size: width |
| 10 | 0f | Window size: height |
| 11 | 00 | Reserved, keep as 0 |
| 12-15 | 00 00 00 00 | Reserved, keep as 0 |

### Configuration byte 0

| Bit index | Description |
|---|---|
| 7,4 | RamSize |
| 6,5 | Graphic mode |
| 0 | Pen input (1) / Keyboard input (0) |

#### RamSize

| Value | Description |
|---|---|
| 0x00 | RamSize = 16 |
| 0x10 | RamSize = 32 |
| 0x80 | RamSize = 24 |

This limits pointer value.

#### Graphic mode

| Value | Description |
|---|---|
| 0x00 | Mode 1 (Monochromatic), FG colour 15 |
| 0x40 | Mode 4 (16-colour), FG colour 15 |
| 0x60 | Mode 8 (256-colour), FG colour 255 |

### Window size

    width = (byte 9) * 16

`width` clamps to `[160, 320]`

    height = (byte 10) * 16

`height` clamps to `[80, 240]`

## General information

(V)RAM size 0x1000000 / 16MiB.

Little-endianness.

Text buffer offset `0x1400`.

(Eval/generic) stack offset `0x1b00`.\
Stack is 4-byte aligned.

String stack offset `0x1c00`.\
String stack is 1-byte aligned.\
String stack is restricted to 768 bytes, wraps around.

Boolean values:\
`LTRUE = 0xffffffff`\
`LFALSE = 0`

## Reset initialisation

PC set to offset 16 (just after file header).

Clear screen.

## Instructions

First byte: opcode

### OP code

#### Set 0

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

#### Set 1

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

### push_char

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x01 |
| 0x01 | u8 | data0 | Data |

Stack push:

| Type | Name | Description |
|---|---|---|
| u32 | data0 | Data |

Push unsigned `data0` onto stack.

### push_int

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x02 |
| 0x01-0x02 | i16 | data0 | Data |

Stack push:

| Type | Name | Description |
|---|---|---|
| i32 | data0 | Data |

Push signed `data0` onto stack.

### push_long

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x03 |
| 0x01-0x04 | u32 | data0 | Data |

Stack push:

| Type | Name | Description |
|---|---|---|
| u32 | data0 | Data |

Push `data0` onto stack.

### push_vchar

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x04 |
| 0x01-0x02/0x03 | addr | ptr0 | RAM pointer 2 or 3 bytes |

Stack push:

| Type | Name | Description |
|---|---|---|
| u32 | data0 | Data |

Push unsigned `data0` at `*ptr0` onto stack.

### push_string

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

### c_let

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

### pop_val

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x38 |

Pop and discard u32 from stack.

### c_jmpe

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x39 |
| +3 | u24 | addr0 | PC address |

```c

void c_jmpe()
{
	if (result==0) {
		c_jmp();
	} else {
		getcode();
		getcode();
		getcode();
	}
}

```

### c_jmp

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x3b |
| +3 | u24 | addr0 | PC address |

Jump to PC address `addr0`.

### set_sp

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x3c |
| +2/+3 | addr | addr0 | Stack pointer address |

Set stack pointer to `addr0`.

### add_bp

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x3e |
| +2/+3 | addr | addr0 | Pointer address |
| +1 | u8 | data0 | Data |

TODO WHAT IS THIS

```c

void add_bp()
{
	long t;
	byte t2;
	int i;
	t=local_bp;
	local_bp=local_sp;
	lRamWrite(local_bp+3,(byte)(t&0xff));
	lRamWrite(local_bp+4,(byte)((t>>8)&0xff));
	if (RamBits>16) lRamWrite(local_bp+5,(byte)((t>>16)&0xff));
	t=getcode();
	t+=getcode()<<8;
	if (RamBits>16) t+=getcode()<<16;
	local_sp=local_bp+(t&0xffffff);
	//if (local_sp&3) local_sp+=4-(local_sp&3); //令堆栈开始于4字节边界
	t=getcode()*4;
	if (t) {
		eval_top-=(word)t;
		t2=eval_top;
		i=0;
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

### cal_qadd

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

### cal_qless

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

### c_block

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

### c_clearscreen

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0x8e |

Clears the entire screen.

### c_fopen

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

### c_fclose

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | u8 | opcode | 0xaf |

Stack pop:

| Type | Name | Description |
|---|---|---|
| u8 | fd | File descriptor |

Close file descriptor.

### c_setgraphmode

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

### c_setbgcolor

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

### c_setfgcolor

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
