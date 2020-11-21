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

#include <filesystem>
#include <sstream>

#include "../include/ext/loguru/loguru.hpp"

#include "../include/vrp_lp.hpp"
#include "../include/instance.hpp"
#include "../include/init_grb_model.hpp"

/////////////////////////////// Helper functions ///////////////////////////////

namespace
{

/**
 * @brief.
*/
void initModel(GRBModel& model,
               std::vector<std::vector<GRBVar>>& y,
               std::vector<std::vector<std::vector<GRBVar>>>& x,
               std::vector<GRBConstr>& constrs,
               const std::shared_ptr<const Instance>& pInst)
{
    RAW_LOG_F(INFO, "Building model...");

    try
    {
        init::variablesY(model, y, pInst);
        init::variablesX(model, x, pInst);

        init::singleVisitationConstrs(model, y, constrs, pInst);
        init::kVehiclesLeaveDepotConstr(model, y, constrs, pInst);
        init::degreeConstrs(model, y, x, constrs, pInst);
        init::vehicleCapacityConstrs(model, y, constrs, pInst);
        init::routeConnectivityConstrs(model, y, x, constrs, pInst);
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

////////////////////////////////////////////////////////////////////////////////

VrpLp::VrpLp(const std::shared_ptr<const Instance>& pInst) :
    mpInst(pInst),
    mModel(mEnv)
{
    initModel(mModel, m_y, m_x, mConstrs, mpInst);
}


void VrpLp::solve()
{
    RAW_LOG_F(INFO, "Solving VRP LP...\n");

    try
    {
        // set solver parameters
        // mModel.set(GRB_IntParam_OutputFlag, params.show_log);
        // mModel.set(GRB_DoubleParam_TimeLimit, params.time_limit);
        // mModel.set(GRB_IntParam_Threads, params.nb_threads);

        mModel.optimize();

        // if (mModel.get(GRB_IntAttr_Status) == GRB_OPTIMAL ||
        //     mModel.get(GRB_IntAttr_Status) == GRB_TIME_LIMIT)
        // {
        //     solved = true;
        // }
    }
    catch (GRBException& e)
    {
        RAW_LOG_F(FATAL, "VRP::solve(): error code: %d", e.getErrorCode());
        RAW_LOG_F(FATAL, "VRP::solve(): C-Exp: %s", e.getMessage().c_str());
    }
    catch (...)
    {
        RAW_LOG_F(FATAL, "VRP::solve(): unknown Exception");
    }
}


void VrpLp::writeIis(std::string path)
{
    DCHECK_F(std::filesystem::is_directory(path), "dir does not exists");
    path += mpInst->getName() + "_vrp.ilp";

    try
    {
        mModel.computeIIS();
        mModel.write(path);
    }
    catch (GRBException& e)
    {
        RAW_LOG_F(ERROR, "writeIis() exp: %s", e.getMessage().c_str());
    }
    catch (...)
    {
        RAW_LOG_F(ERROR, "writeIis(): Unknown Exception");
    }
}


void VrpLp::writeSolution(std::string path)
{
    DCHECK_F(std::filesystem::is_directory(path), "dir does not exists");
    path += mpInst->getName() + ".sol";

    try
    {
        mModel.write(path);
    }
    catch (GRBException& e)
    {
        RAW_LOG_F(ERROR, "writeSolution() exp: %s", e.getMessage().c_str());
    }
    catch (...)
    {
        RAW_LOG_F(ERROR, "writeSolution(): Unknown Exception");
    }
}