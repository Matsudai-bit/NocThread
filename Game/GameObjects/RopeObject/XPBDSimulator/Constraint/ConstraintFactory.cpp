/*****************************************************************//**
 * @file    ConstraintFactoryBase.cpp
 * @brief   制約の生成インターフェースに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/10/09
 *********************************************************************/

#include "pch.h"
#include "ConstraintFactory.h"

ConstraintFactoryBase::ConstraintFactoryBase(bool isDynamic)
	: m_isDynamic{ isDynamic }
{
}
