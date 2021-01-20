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
    const std::vector<std::vector<std::vector<GRBVar>>>& x,
    const std::vector<std::vector<GRBVar>>& y,
    const std::shared_ptr<const Instance>& p_inst) :
        m_x(x),
        m_y(y),
        mpInst(p_inst)
{}


void CallbackSEC::callback()
{
    try
    {
        switch (where)
        {
        case GRB_CB_MIPSOL:
        {
            addLazyCVRPSEP();
            break;
        }
        case GRB_CB_MIPNODE:
        {
            if (getIntInfo(GRB_CB_MIPNODE_STATUS) == GRB_OPTIMAL)
            {
                addCutCVRPSEP();
            }
            break;
        }
        default:
            break;
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


void CallbackSEC::addLazyCVRPSEP()
{
    std::vector<std::vector<std::vector<double>>> valueX(
        mpInst->getNbVertices(),
        std::vector<std::vector<double>>(
            mpInst->getNbVertices(),
            std::vector<double>(mpInst->getK(), 0)));

    /* get solution: routing */
    for (int i = 0; i < mpInst->getNbVertices(); ++i)
    {
        for (int j = i + 1; j < mpInst->getNbVertices(); ++j)
        {
            for (int k = 0; k < mpInst->getK(); ++k)
            {
                valueX[i][j][k] = getSolution(m_x[i][j][k]);
            }
        }
    }

    for (int k = 0; k < mpInst->getK(); ++k)
    {
        int nbEdges = 0;
        for (int i = 0; i < mpInst->getNbVertices(); ++i)
        {
            for (int j = i + 1; j < mpInst->getNbVertices(); ++j)
            {
                if (valueX[i][j][k] > C_EPS)
                {
                    ++nbEdges;
                }
            }
        }

        if (nbEdges > 0)
        {
            /* Parameters of the CVRPSEP */
            const int maxNbCuts = 8;
            char integerAndFeasible;
            double maxViolation = 0;

            std::vector<int> demand(mpInst->getNbVertices());

            for (int i = 1; i < mpInst->getNbVertices(); ++i)
            {
                demand[i] = mpInst->getdi(i);
            }

            std::vector<int> edgeTail(nbEdges + 1, 0);
            std::vector<int> edgeHead(nbEdges + 1, 0);
            std::vector<double> edgeX(nbEdges + 1, 0);

            int aux = 1;
            for (int i = 0; i < mpInst->getNbVertices(); ++i)
            {
                for (int j = i + 1; j < mpInst->getNbVertices(); ++j)
                {
                    if (valueX[i][j][k] > C_EPS)
                    {
                        if (i == 0)
                        {
                            edgeTail[aux] = mpInst->getNbVertices();
                        }
                        else
                        {
                            edgeTail[aux] = i;
                        }
                        edgeHead[aux] = j;
                        edgeX[aux] = valueX[i][j][k];
                        ++aux;
                    }
                }
            }

            const int dim = 100;
            CnstrMgrPointer myCutsCMP, myOldCutsCMP;
            CMGR_CreateCMgr(&myCutsCMP, dim);
            CMGR_CreateCMgr(&myOldCutsCMP, dim);

            CAPSEP_SeparateCapCuts(mpInst->getNbVertices() - 1,
                                   demand.data(),
                                   mpInst->getC(),
                                   nbEdges,
                                   edgeTail.data(),
                                   edgeHead.data(),
                                   edgeX.data(),
                                   myOldCutsCMP,
                                   maxNbCuts,
                                   C_EPS,
                                   &integerAndFeasible,
                                   &maxViolation,
                                   myCutsCMP);

            /* capacity */
            std::vector<int> list(mpInst->getNbVertices(), 0);

            for (int cut = 0; cut < myCutsCMP->Size; ++cut)
            {
                if (myCutsCMP->CPL[cut]->CType == CMGR_CT_CAP)
                {
                    /* capacity cut */
                    int listSize = 0;
                    for (int j = 1; j <= myCutsCMP->CPL[cut]->IntListSize; ++j)
                    {
                        list[++listSize] = myCutsCMP->CPL[cut]->IntList[j];
                    }

                    GRBLinExpr e1 = 0;
                    for (int i = 1; i <= listSize; ++i)
                    {
                        for (int j = 1; j <= listSize; ++j)
                        {
                            if (list[i] < list[j])
                            {
                                e1 += m_x[list[i]][list[j]][k];
                            }
                        }
                    }

                    GRBLinExpr e2 = 0;
                    for (int i = 1; i <= listSize; ++i)
                    {
                        e2 += m_y[list[i]][k];
                    }

                    for (int i = 1; i <= listSize; ++i)
                    {
                        addLazy(e1 <= e2 - m_y[list[i]][k]);
                    }
                }
            }

            CMGR_FreeMemCMgr(&myCutsCMP);
            CMGR_FreeMemCMgr(&myOldCutsCMP);
        }
    }
}


void CallbackSEC::addCutCVRPSEP()
{
    std::vector<std::vector<double>> valueY(std::vector<std::vector<double>>(
            mpInst->getNbVertices(), std::vector<double>(mpInst->getK(), 0)));

    /* get solution: routing */
    for (int i = 0; i < mpInst->getNbVertices(); ++i)
    {
        for (int k = 0; k < mpInst->getK(); ++k)
        {
            valueY[i][k] = getNodeRel(m_y[i][k]);
        }
    }

    std::vector<std::vector<std::vector<double>>> valueX(
        mpInst->getNbVertices(),
        std::vector<std::vector<double>>(
            mpInst->getNbVertices(),
            std::vector<double>(mpInst->getK(), 0)));

    /* get solution: routing */
    for (int i = 0; i < mpInst->getNbVertices(); ++i)
    {
        for (int j = i + 1; j < mpInst->getNbVertices(); ++j)
        {
            for (int k = 0; k < mpInst->getK(); ++k)
            {
                valueX[i][j][k] = getNodeRel(m_x[i][j][k]);
            }
        }
    }

    for (int k = 0; k < mpInst->getK(); ++k)
    {
        int nbEdges = 0;
        for (int i = 0; i < mpInst->getNbVertices(); ++i)
        {
            for (int j = i + 1; j < mpInst->getNbVertices(); ++j)
            {
                if (valueX[i][j][k] > C_EPS)
                {
                    ++nbEdges;
                }
            }
        }

        if (nbEdges > 0)
        {
            /* Parameters of the CVRPSEP */
            std::vector<int> demand(mpInst->getNbVertices(), 0);
            std::vector<int> edgeTail(nbEdges + 1);
            std::vector<int> edgeHead(nbEdges + 1);
            std::vector<double> edgeX(nbEdges + 1);

            char integerAndFeasible;
            double maxViolation = 0;

            const int maxNbCapCuts = 8;

            for (int i = 1; i < mpInst->getNbVertices(); ++i)
            {
                demand[i] = mpInst->getdi(i);
            }

            int aux = 1;
            for (int i = 0; i < mpInst->getNbVertices(); ++i)
            {
                for (int j = i + 1; j < mpInst->getNbVertices(); ++j)
                {
                    if (valueX[i][j][k] > C_EPS)
                    {
                        if (i == 0)
                        {
                            edgeTail[aux] = mpInst->getNbVertices();
                        }
                        else
                        {
                            edgeTail[aux] = i;
                        }
                        edgeHead[aux] = j;
                        edgeX[aux] = valueX[i][j][k];
                        ++aux;
                    }
                }
            }

            const int dim = 100;
            CnstrMgrPointer myCutsCMP, myOldCutsCMP;
            CMGR_CreateCMgr(&myCutsCMP, dim);
            CMGR_CreateCMgr(&myOldCutsCMP, dim);

            CAPSEP_SeparateCapCuts(mpInst->getNbVertices() - 1,
                                   demand.data(),
                                   mpInst->getC(),
                                   nbEdges,
                                   edgeTail.data(),
                                   edgeHead.data(),
                                   edgeX.data(),
                                   myOldCutsCMP,
                                   maxNbCapCuts,
                                   C_EPS,
                                   &integerAndFeasible,
                                   &maxViolation,
                                   myCutsCMP);

            /* capacity */
            std::vector<int> list(mpInst->getNbVertices(), 0);

            for (int cut = 0; cut < myCutsCMP->Size; ++cut)
            {
                if (myCutsCMP->CPL[cut]->CType == CMGR_CT_CAP)
                {
                    /* capacity cut */
                    int listSize = 0;
                    for (int j = 1; j <= myCutsCMP->CPL[cut]->IntListSize; ++j)
                    {
                        list[++listSize] = myCutsCMP->CPL[cut]->IntList[j];
                    }

                    //double rhs = myCutsCMP->CPL[cut]->RHS;
                    GRBLinExpr e1 = 0;
                    double sumX = 0;
                    for (int i = 1; i <= listSize; ++i)
                    {
                        for (int j = 1; j <= listSize; ++j)
                        {
                            if (list[i] < list[j])
                            {
                                e1 += m_x[list[i]][list[j]][k];
                                sumX += valueX[list[i]][list[j]][k];
                            }
                        }
                    }

                    GRBLinExpr e2 = 0;
                    double sumY = 0;
                    for (int i = 1; i <= listSize; ++i)
                    {
                        e2 += m_y[list[i]][k];
                        sumY += valueY[list[i]][k];
                    }

                    for (int i = 1; i <= listSize; ++i)
                    {
                        if (sumX >= (sumY - valueY[list[i]][k]))
                        {
                            addCut(e1 <= e2 - m_y[list[i]][k]);
                        }
                    }
                }
            }

            CMGR_FreeMemCMgr(&myCutsCMP);
            CMGR_FreeMemCMgr(&myOldCutsCMP);
        }
    }
}