////////////////////////////////////////////////////////////////////////////////
/*
 * File: callback_sec.cpp
 * Author: Guilherme O. Chagas
 *
 * @brief Callback class definition for lazy/cut subtour separation
 * constraints.
 *
 * @acknowledgment Special thanks to Ph.D. Cleder Marcos Schenekemberg.
 *
 * (I'm sorry for my bad english xD)
 *
 * Created on November 16, 2020, 00:15 AM
 * 
 * References:
 */
////////////////////////////////////////////////////////////////////////////////

#include "../../include/ext/loguru/loguru.hpp"

#include "../../include/callback/callback_sec.hpp"
#include "../../include/ext/cvrpsep/capsep.h"
#include "../../include/ext/cvrpsep/cnstrmgr.h"

////////////////////////////////////////////////////////////////////////////////

namespace
{

static const int C_EPS = 1e-5;

} // anonymous namespace

////////////////////////////////////////////////////////////////////////////////

CallbackSEC::CallbackSEC(
    const utils::Vec3D<GRBVar>& x,
    const utils::Vec2D<GRBVar> & y,
    const std::shared_ptr<const Instance>& p_inst) :
        mr_x(x),
        mr_y(y),
        mpInst(p_inst)
{}


void CallbackSEC::callback()
{
    try
    {
        if (where == GRB_CB_MIPSOL)
        {
            addCVRPSEPCAP(constrsType::lazy);
        }
        else if (where == GRB_CB_MIPNODE &&
                 getIntInfo(GRB_CB_MIPNODE_STATUS) == GRB_OPTIMAL)
        {
            addCVRPSEPCAP(constrsType::cut);
        }
    }
    catch (GRBException& e)
    {
        RAW_LOG_F(ERROR, "callback() exp: %s", e.getMessage().c_str());
    }
    catch (...)
    {
        RAW_LOG_F(ERROR, "callback(): Unknown Exception");
    }
}

/* -------------------------------------------------------------------------- */


std::vector<std::vector<std::vector<double>>>
    CallbackSEC::getxVarsValues(const constrsType cstType)
{
    std::vector<std::vector<std::vector<double>>> xVal(
        mpInst->getNbVertices(), std::vector<std::vector<double>>(
            mpInst->getNbVertices(), std::vector<double>(mpInst->getK(), 0)
        )
    );

    for (int i = 0; i < mpInst->getNbVertices(); ++i)
    {
        for (int j = i + 1; j < mpInst->getNbVertices(); ++j)
        {
            for (int k = 0; k < mpInst->getK(); ++k)
            {
                DCHECK_F(i < static_cast<int>(mr_x.size()));
                DCHECK_F(j < static_cast<int>(mr_x[i].size()));
                DCHECK_F(k < static_cast<int>(mr_x[i][j].size()));
                if (cstType == constrsType::lazy)
                {
                    xVal[i][j][k] = getSolution(mr_x[i][j][k]);
                }
                else
                {
                    xVal[i][j][k] = getNodeRel(mr_x[i][j][k]);
                }
            }
        }
    }

    return xVal;
}


std::vector<std::vector<double>> CallbackSEC::getyVarsValues(
        const constrsType cstType)
{
    std::vector<std::vector<double>> yVal(
        mpInst->getNbVertices(), std::vector<double>(mpInst->getK(), 0)
    );

    for (int i = 0; i < mpInst->getNbVertices(); ++i)
    {
        for (int k = 0; k < mpInst->getK(); ++k)
        {
            DCHECK_F(i < static_cast<int>(mr_y.size()));
            DCHECK_F(k < static_cast<int>(mr_y[i].size()));
            if (cstType == constrsType::lazy)
            {
                yVal[i][k] = getSolution(mr_y[i][k]);
            }
            else
            {
                yVal[i][k] = getNodeRel(mr_y[i][k]);
            }
        }
    }

    return yVal;
}