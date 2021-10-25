/*
Copyright (C) 2001,2006 StrmnNrmn

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#ifndef CORE_R4300OPCODE_H_
#define CORE_R4300OPCODE_H_

#include "Base/Types.h"
#include "System/Endian.h"
#include "N64Reg.h"

enum class OpCodeValue {
	SPECOP		= 0,
	REGIMM		= 1,
	J			= 2,
	JAL			= 3,
	BEQ			= 4,
	BNE			= 5,
	BLEZ		= 6,
	BGTZ		= 7,
	ADDI		= 8,
	ADDIU		= 9,
	SLTI		= 10,
	SLTIU		= 11,
	ANDI		= 12,
	ORI			= 13,
	XORI		= 14,
	LUI			= 15,
	COPRO0		= 16,
	COPRO1		= 17,
	SRHACK1		= 18,	//hack OP
	SRHACK2		= 19,	//hack OP
	BEQL		= 20,
	BNEL		= 21,
	BLEZL		= 22,
	BGTZL		= 23,
	DADDI		= 24,
	DADDIU		= 25,
	LDL			= 26,
	LDR			= 27,
	PATCH		= 28,	//hack OP
	UNOPT		= 29,	//hack OP
	OPT  		= 30,	//hack OP
	NOOPT 		= 31,	//hack OP
	UNK4		= 29,	//unknown OP
	UNK5		= 30,	//unknown OP
	UNK6		= 31,	//unknown OP
	LB			= 32,
	LH			= 33,
	LWL			= 34,
	LW 			= 35,
	LBU			= 36,
	LHU			= 37,
	LWR			= 38,
	LWU			= 39,
	SB			= 40,
	SH			= 41,
	SWL			= 42,
	SW			= 43,
	SDL			= 44,
	SDR			= 45,
	SWR			= 46,
	CACHE		= 47,
	LL			= 48,
	LWC1		= 49,
	UNK7		= 50,	//unknown OP
	UNK8		= 51,	//unknown OP
	LLD			= 52,
	LDC1		= 53,
	LDC2 		= 54,	//not used OP
	LD			= 55,
	OP_SC 		= 56,
	SWC1 		= 57,
	DBG_BKPT	= 58,	//unknown OP
	OP_UNK10	= 59,	//unknown OP
	OP_SCD 		= 60,
	SDC1 		= 61,
	OP_SDC2 	= 62,	//not used OP
	SD			= 63
};

enum ESpecOp
{
	SpecOp_SLL				= 0,
//	SpecOp_UNK11			= 1,
	SpecOp_SRL				= 2,
	SpecOp_SRA				= 3,
	SpecOp_SLLV				= 4,
//	SpecOp_UNK12			= 5,
	SpecOp_SRLV				= 6,
	SpecOp_SRAV				= 7,
	SpecOp_JR				= 8,
	SpecOp_JALR				= 9,
/*	SpecOp_UNK13			= 10,
	SpecOp_UNK14			= 11,*/
	SpecOp_SYSCALL			= 12,
	SpecOp_BREAK			= 13,
//	SpecOp_UNK15			= 14,
	SpecOp_SYNC				= 15,
	SpecOp_MFHI				= 16,
	SpecOp_MTHI				= 17,
	SpecOp_MFLO				= 18,
	SpecOp_MTLO				= 19,
	SpecOp_DSLLV			= 20,
//	SpecOp_UNK16			= 21,
	SpecOp_DSRLV			= 22,
	SpecOp_DSRAV			= 23,
	SpecOp_MULT				= 24,
	SpecOp_MULTU			= 25,
	SpecOp_DIV				= 26,
	SpecOp_DIVU				= 27,
	SpecOp_DMULT			= 28,
	SpecOp_DMULTU			= 29,
	SpecOp_DDIV				= 30,
	SpecOp_DDIVU			= 31,
	SpecOp_ADD				= 32,
	SpecOp_ADDU				= 33,
	SpecOp_SUB				= 34,
	SpecOp_SUBU				= 35,
	SpecOp_AND				= 36,
	SpecOp_OR				= 37,
	SpecOp_XOR				= 38,
	SpecOp_NOR				= 39,
/*	SpecOp_UNK17			= 40,
	SpecOp_UNK18			= 41,*/
	SpecOp_SLT				= 42,
	SpecOp_SLTU				= 43,
	SpecOp_DADD				= 44,
	SpecOp_DADDU			= 45,
	SpecOp_DSUB				= 46,
	SpecOp_DSUBU			= 47,
	SpecOp_TGE				= 48,
	SpecOp_TGEU				= 49,
	SpecOp_TLT				= 50,
	SpecOp_TLTU				= 51,
	SpecOp_TEQ				= 52,
//	SpecOp_UNK19			= 53,
	SpecOp_TNE				= 54,
//	SpecOp_UNK20			= 55,
	SpecOp_DSLL				= 56,
//	SpecOp_UNK21			= 57,
	SpecOp_DSRL				= 58,
	SpecOp_DSRA				= 59,
	SpecOp_DSLL32			= 60,
//	SpecOp_UNK22			= 61,
	SpecOp_DSRL32			= 62,
	SpecOp_DSRA32			= 63,

};

enum ERegImmOp
{
	RegImmOp_BLTZ			= 0,
	RegImmOp_BGEZ			= 1,
	RegImmOp_BLTZL			= 2,
	RegImmOp_BGEZL			= 3,
	RegImmOp_TGEI			= 8,
	RegImmOp_TGEIU			= 9,
	RegImmOp_TLTI			= 10,
	RegImmOp_TLTIU			= 11,
	RegImmOp_TEQI			= 12,
	RegImmOp_TNEI			= 14,
	RegImmOp_BLTZAL			= 16,
	RegImmOp_BGEZAL			= 17,
	RegImmOp_BLTZALL		= 18,
	RegImmOp_BGEZALL		= 19
};

enum class ECop0Op
{
	MFC0		= 0,
	MTC0		= 4,
	TLB		= 16
};

enum class TLBOpCodeValue {
	TLBR = 1, 
	TLBWI = 2,
	TLBWR = 6,
	TLBBP = 8,
	ERET = 24
};

enum ECop1Op
{
	Cop1Op_MFC1	= 0,
	Cop1Op_DMFC1	= 1,
	Cop1Op_CFC1	= 2,
	Cop1Op_MTC1	= 4,
	Cop1Op_DMTC1	= 5,
	Cop1Op_CTC1	= 6,

	Cop1Op_BCInstr = 8,
	Cop1Op_SInstr = 16,
	Cop1Op_DInstr = 17,
	Cop1Op_WInstr = 20,
	Cop1Op_LInstr = 21

};

enum class ECop1OpFunction
{
	ADD = 0,
	SUB = 1,
	MUL = 2,
	DIV = 3,
	SQRT = 4,
	ABS = 5,
	MOV = 6,
	NEG = 7,

	ROUND_L = 8,
	TRUNC_L = 9,
	CEIL_L = 10,
	FLOOR_L = 11,
	ROUND_W = 12,
	TRUNC_W = 13,
	CEIL_W = 14,
	FLOOR_W = 15,

	CVT_S = 32,
	CVT_D = 33,
	CVT_W = 36,
	CVT_L = 37,

	CMP_F = 48,
	CMP_UN = 49,
	CMP_EQ = 50,
	CMP_UEQ = 51,
	CMP_OLT = 52,
	CMP_ULT = 53,
	CMP_OLE = 54,
	CMP_ULE = 55,

	CMP_SF = 56,
	CMP_NGLE = 57,
	CMP_SEQ = 58,
	CMP_NGL = 59,
	CMP_LT = 60,
	CMP_NGE = 61,
	CMP_LE = 62,
	CMP_NGT = 63,
};

enum class ECop1BCOp
{
	BC1F = 0,
	BC1T = 1,
	BC1FL = 2,
	BC1TL = 3
};

struct OpCode
{
	union
	{
		u32 _u32;

#if (DAEDALUS_ENDIAN_MODE == DAEDALUS_ENDIAN_BIG)
		struct
		{
			unsigned op : 6;
			unsigned rs : 5;			// EN64Reg
			unsigned rt : 5;			// EN64Reg
			unsigned offset : 16;
		};

		struct
		{
			unsigned : 6;
			unsigned base : 5;			// EN64Reg
			unsigned : 5;
			unsigned immediate : 16;
		};

		struct
		{
			unsigned : 6;
			unsigned target : 26;
		};

		// SPECIAL
		struct
		{
			unsigned : 6;
			unsigned : 5;
			unsigned : 5;
			unsigned rd : 5;			// EN64Reg
			unsigned sa : 5;
			unsigned spec_op : 6;		// ESpecOp
		};

		// REGIMM
		struct
		{
			unsigned : 11;
			unsigned regimm_op : 5;		// ERegImmOp
			unsigned : 16;
		};

		// COP0 op
		struct
		{
			unsigned : 6;
			unsigned cop0_op : 5;		// ECop0Op
			unsigned : 21;
		};

		// COP1 op
		struct
		{
			unsigned : 6;
			unsigned cop1_op : 5;
			unsigned ft : 5;		// rt
			unsigned fs : 5;		// rd
			unsigned fd : 5;		// sa
			unsigned cop1_funct : 6;
		};


		struct
		{
			unsigned : 14;
			unsigned cop1_bc : 2;		// ECop1BCOp
			unsigned : 16;
		};

		struct
		{
			unsigned : 6;
			unsigned : 5;
			unsigned dest   : 5;
			unsigned : 5;
			unsigned del    : 4;
			signed   voffset : 7;
		};

		// COP0 TLB op
		struct
		{
			unsigned : 26;
			unsigned cop0tlb_funct : 6;
		};

		// COP2 vector op
		struct
		{
			unsigned : 26;
			unsigned cop2_funct : 6;
		};

		struct
		{
			unsigned : 6;
			unsigned bp_index : 26;
		};

		struct
		{
			unsigned : 6;
			unsigned dynarec_index : 26;
		};

		struct
		{
			unsigned : 6;
			unsigned trace_branch_index : 26;
		};

		struct
		{
			unsigned : 6;
			unsigned : 2;
			unsigned patch_index : 24;
		};
#elif (DAEDALUS_ENDIAN_MODE == DAEDALUS_ENDIAN_LITTLE)
		struct
		{
			unsigned offset : 16;
			unsigned rt : 5;			// EN64Reg
			unsigned rs : 5;			// EN64Reg
			unsigned op : 6;
		};

		struct
		{
			unsigned immediate : 16;
			unsigned : 5;
			unsigned base : 5;			// EN64Reg
			unsigned : 6;
		};

		struct
		{
			unsigned target : 26;
			unsigned : 6;
		};

		// SPECIAL
		struct
		{
			unsigned spec_op : 6;		// ESpecOp
			unsigned sa : 5;
			unsigned rd : 5;			// EN64Reg
			unsigned : 5;
			unsigned : 5;
			unsigned : 6;
		};

		// REGIMM
		struct
		{
			unsigned : 16;
			unsigned regimm_op : 5;		// ERegImmOp
			unsigned : 11;
		};

		// COP0 op
		struct
		{
			unsigned : 21;
			unsigned cop0_op : 5;		// ECop0Op
			unsigned : 6;
		};

		// COP1 op
		struct
		{
			unsigned cop1_funct : 6;
			unsigned fd : 5;		// sa
			unsigned fs : 5;		// rd
			unsigned ft : 5;		// rt
			unsigned cop1_op : 5;
			unsigned : 6;
		};


		struct
		{
			unsigned : 16;
			unsigned cop1_bc : 2;		// ECop1BCOp
			unsigned : 14;
		};

		struct
		{
			signed   voffset : 7;
			unsigned del    : 4;
			unsigned : 5;
			unsigned dest   : 5;
			unsigned : 5;
			unsigned : 6;
		};

		// COP0 TLB op
		struct
		{
			unsigned cop0tlb_funct : 6;
			unsigned : 26;
		};

		// COP2 vector op
		struct
		{
			unsigned cop2_funct : 6;
			unsigned : 26;
		};

		struct
		{
			unsigned bp_index : 26;
			unsigned : 6;
		};

		struct
		{
			unsigned dynarec_index : 26;
			unsigned : 6;
		};

		struct
		{
			unsigned trace_branch_index : 26;
			unsigned : 6;
		};

		struct
		{
			unsigned patch_index : 24;
			unsigned : 2;
			unsigned : 6;
		};
#else
#error No DAEDALUS_ENDIAN_MODE specified
#endif

	};
};

// Make sure we don't mess this up :)
DAEDALUS_STATIC_ASSERT( sizeof( OpCode ) == sizeof( u32 ) );

#endif // CORE_R4300OPCODE_H_
