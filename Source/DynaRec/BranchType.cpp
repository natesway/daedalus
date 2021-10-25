/*
Copyright (C) 2006 StrmnNrmn

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

#include "stdafx.h"


#include <stdlib.h>

#include "Core/R4300OpCode.h"
#include "DynaRec/BranchType.h"
#include "Base/Macros.h"

//*************************************************************************************
//
//*************************************************************************************
/*
static const ER4300BranchType gInverseBranchTypes[] =
{
	BT_NOT_BRANCH,
	BT_BNE,		BT_BNEL,
	BT_BEQ,		BT_BEQL,
	BT_BGTZ,	BT_BGTZL,
	BT_BLEZ,	BT_BLEZL,
	BT_BGEZ,	BT_BGEZL,	BT_BGEZAL,	BT_BGEZALL,
	BT_BLTZ,	BT_BLTZL,	BT_BLTZAL,	BT_BLTZALL,
	BT_BC1T,	BT_BC1TL,
	BT_BC1F,	BT_BC1FL,
	BT_J,		// All the following are unconditional
	BT_JAL,
	BT_JR,
	BT_JALR,
	BT_ERET,
};

*/

//*************************************************************************************
//
//*************************************************************************************
/*
OpCode	GetInverseBranch( OpCode op_code )
{
	switch( op_code.op )
	{
	case OpCodeValue::J:				break;
	case OpCodeValue::JAL:			break;
	case OpCodeValue::BEQ:			op_code.op = OpCodeValue::BNE;	break;
	case OpCodeValue::BNE:			op_code.op = OpCodeValue::BEQ;	break;
	case OpCodeValue::BLEZ:			op_code.op = OpCodeValue::BGTZ;	break;
	case OpCodeValue::BGTZ:			op_code.op = OpCodeValue::BLEZ;	break;
	case OpCodeValue::BEQL:			op_code.op = OpCodeValue::BNEL;	break;
	case OpCodeValue::BNEL:			op_code.op = OpCodeValue::BEQL;	break;
	case OpCodeValue::BLEZL:			op_code.op = OpCodeValue::BGTZL;	break;
	case OpCodeValue::BGTZL:			op_code.op = OpCodeValue::BLEZL;	break;

	case OpCodeValue::REGIMM:
		switch( op_code.regimm_op )
		{
		case RegImmOp_BLTZ:		op_code.regimm_op = RegImmOp_BGEZ;	break;
		case RegImmOp_BGEZ:		op_code.regimm_op = RegImmOp_BLTZ;	break;
		case RegImmOp_BLTZL:	op_code.regimm_op = RegImmOp_BGEZL;	break;
		case RegImmOp_BGEZL:	op_code.regimm_op = RegImmOp_BLTZL;	break;
		case RegImmOp_BLTZAL:	op_code.regimm_op = RegImmOp_BGEZAL;break;
		case RegImmOp_BGEZAL:	op_code.regimm_op = RegImmOp_BLTZAL;break;
		case RegImmOp_BLTZALL:	op_code.regimm_op = RegImmOp_BGEZALL;break;
		case RegImmOp_BGEZALL:	op_code.regimm_op = RegImmOp_BLTZALL;break;
		default:
			NODEFAULT;
			break;
		}
		break;

	case OpCodeValue::SPECOP:
		switch( op_code.spec_op )
		{
		case SpecOp_JR:			break;
		case SpecOp_JALR:		break;
		default:
			break;
		}
		break;

	case OpCodeValue::COPRO0:
		if( op_code.cop0_op == ECop0Op::TLB )
		{
			switch( op_code.cop0tlb_funct )
			{
			case TLBOpCodeValue::ERET:	break;
			}
		}
		break;
	case OpCodeValue::COPRO1:
		if( op_code.cop1_op == Cop1Op_BCInstr )
		{
			switch( op_code.cop1_bc )
			{
			case ECop1BCOp::BC1F:		op_code.cop1_bc = ECop1BCOp::BC1T;	break;
			case ECop1BCOp::BC1T:		op_code.cop1_bc = ECop1BCOp::BC1F;	break;
			case ECop1BCOp::BC1FL:	op_code.cop1_bc = ECop1BCOp::BC1TL;	break;
			case ECop1BCOp::BC1TL:	op_code.cop1_bc = ECop1BCOp::BC1FL;	break;
			}
		}
		break;
	}

	return op_code;
}
*/
//*************************************************************************************
//
//*************************************************************************************
/*
namespace
{
	OpCode	UpdateBranchOffset( OpCode op_code, u32 branch_location, u32 target_location )
	{
		DAEDALUS_ASSERT( (target_location & 0x3) == 0, "Target location is not 4-byte aligned!" );

		s32		offset( target_location - branch_location );		// signed

		// XXXX check if jump is out of range!

		op_code.offset = u16( ( offset - 4 ) >> 2 );
		return op_code;
	}

	OpCode	UpdateJumpTarget( OpCode op_code, u32 jump_location, u32 target_location )
	{
		op_code.target = (target_location - jump_location) >> 2;
		return op_code;
	}
}
*/
//*************************************************************************************
//
//*************************************************************************************
/*
OpCode	UpdateBranchTarget( OpCode op_code, u32 op_address, u32 target_address )
{
	switch( op_code.op )
	{
	case OpCodeValue::J:
	case OpCodeValue::JAL:
		op_code = UpdateJumpTarget( op_code, op_address, target_address );
		break;
	case OpCodeValue::BEQ:
	case OpCodeValue::BNE:
	case OpCodeValue::BLEZ:
	case OpCodeValue::BGTZ:
	case OpCodeValue::BEQL:
	case OpCodeValue::BNEL:
	case OpCodeValue::BLEZL:
	case OpCodeValue::BGTZL:
		op_code = UpdateBranchOffset( op_code, op_address, target_address );
		break;

	case OpCodeValue::REGIMM:
		switch( op_code.regimm_op )
		{
		case RegImmOp_BLTZ:
		case RegImmOp_BGEZ:
		case RegImmOp_BLTZL:
		case RegImmOp_BGEZL:
		case RegImmOp_BLTZAL:
		case RegImmOp_BGEZAL:
		case RegImmOp_BLTZALL:
		case RegImmOp_BGEZALL:
			op_code = UpdateBranchOffset( op_code, op_address, target_address );
			break;
		default:
			NODEFAULT;
			break;
		}
		break;

	case OpCodeValue::SPECOP:
		switch( op_code.spec_op )
		{
		case SpecOp_JR:
		case SpecOp_JALR:
			// No jump target - it's indirect
			break;
		default:
			break;
		}
		break;

	case OpCodeValue::COPRO0:
		if( op_code.cop0_op == ECop0Op::TLB )
		{
			switch( op_code.cop0tlb_funct )
			{
			// No jump target - it's indirect
			case TLBOpCodeValue::ERET:
				break;
			}
		}
		break;
	case OpCodeValue::COPRO1:
		if( op_code.cop1_op == Cop1Op_BCInstr )
		{
			switch( op_code.cop1_bc )
			{
			case ECop1BCOp::BC1F:
			case ECop1BCOp::BC1T:
			case ECop1BCOp::BC1FL:
			case ECop1BCOp::BC1TL:
				op_code = UpdateBranchOffset( op_code, op_address, target_address );
				break;
			}
		}
		break;
	}

	return op_code;
}
*/

//*************************************************************************************
//
//*************************************************************************************
/*
ER4300BranchType	GetInverseBranch( ER4300BranchType type )
{
	ER4300BranchType	inverse( gInverseBranchTypes[ type ] );

	DAEDALUS_ASSERT( gInverseBranchTypes[ inverse ] == type, "Inconsistant inverse branch type" );

	return inverse;
}
*/
//*************************************************************************************
//
//*************************************************************************************

// From PrintOpCode
#define BranchAddress(op, address) (    (address)+4 + (s16)(((op).immediate))*4)
#define JumpTarget(op, address)    (   ((address) & 0xF0000000) | (((op).target)<<2)   )

u32 GetBranchTarget( u32 address, OpCode op_code, ER4300BranchType type )
{
	#ifdef DAEDALUS_ENABLE_ASSERTS
	DAEDALUS_ASSERT( type != BT_NOT_BRANCH, "This is not a valid branch type" );
#endif
	// We pass the type in for efficiency - check that it's correct in debug though
	// This already checked
	//DAEDALUS_ASSERT( GetBranchType( op_code ) == type, "Specified type is inconsistant with op code" );

	if( type < BT_J )
	{
		return BranchAddress( op_code, address );
	}

	// All the following are unconditional
	if( type < BT_JR )
	{
		return JumpTarget( op_code, address );
	}

	// These are all indirect
	return  0;
}
