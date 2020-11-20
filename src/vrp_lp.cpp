////////////////////////////////////////////////////////////////////////////////
/*
 * File: vrp_lp.cpp
 * Author: Guilherme O. Chagas
 *
 * @brief CVRP class definition.
 *
 * (I'm sorry for my bad english xD)
 *
 * Created on November 1, 2020, 03:59 PM.
 * 
 * References:
 */
////////////////////////////////////////////////////////////////////////////////

#include <sstream>

#include "../include/ext/loguru/loguru.hpp"

#include "../include/vrp_lp.hpp"
#include "../include/instance.hpp"
#include "../include/init_grb_model.hpp"


namespace
{

/**
 * @brief.
*/
void initModel(GRBModel& model,
               std::vector<std::vector<GRBVar>>& y,
               std::vector<std::vector<std::vector<GRBVar>>>& x,
               const std::shared_ptr<const Instance>& pInst)
{
    RAW_LOG_F(INFO, "Building model...");

    try
    {
        init::variablesY(model, y, pInst);
        init::variablesX(model, x, pInst);
    }
    catch (GRBException e)
    {
        RAW_LOG_F(FATAL, "VRP::initModel(): error code: %d", e.getErrorCode());
        RAW_LOG_F(FATAL, "VRP::initModel(): C-Exp: %s", e.getMessage().c_str());
    }
    catch (...)
    {
        RAW_LOG_F(FATAL, "VRP::initModel(): unknown Exception");
    }
}

} // anonymous namespace


VrpLp::VrpLp(const std::shared_ptr<const Instance>& pInst) :
    mpInst(pInst),
    mModel(mEnv)
{
    initModel(mModel, m_y, m_x, mpInst);
}
