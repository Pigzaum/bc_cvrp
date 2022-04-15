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
 * [1] https://bit.ly/2Kcnm6C
 * [2] https://bit.ly/2N6iMbl
 */
////////////////////////////////////////////////////////////////////////////////

#include <filesystem>
#include <sstream>

#include "../include/ext/loguru/loguru.hpp"

#include "../include/vrp_lp.hpp"
#include "../include/instance.hpp"
#include "../include/init_grb_model.hpp"
#include "../include/callback/callback_sec.hpp"

/////////////////////////////// Helper functions ///////////////////////////////

namespace
{

/**
 * @brief.
*/
void initModel(GRBModel& model,
               utils::Vec2D<GRBVar>& y,
               utils::Vec2D<GRBVar>& u,
               utils::Vec3D<GRBVar>& x,
               std::vector<GRBConstr>& constrs,
               std::shared_ptr<CallbackSEC> &pCbSEC,
               const std::shared_ptr<const Instance>& pInst,
               const ConfigParameters::model& params)
{
    RAW_LOG_F(INFO, "Building model...");

    try
    {
        init::variablesY(model, y, pInst);
        init::variablesX(model, x, pInst);

        init::singleVisitationConstrs(model, constrs, y, pInst);
        init::kVehiclesLeaveDepotConstr(model, constrs, y, pInst);
        init::degreeConstrs(model, constrs, y, x, pInst);
        init::vehicleCapacityConstrs(model, constrs, y, pInst);

        switch (params.sec_strategy)
        {
        case ConfigParameters::model::sec_opt::CON :
        {
            init::routeConnectivityConstrs(model, constrs, y, x, pInst);
            break;
        }
        case ConfigParameters::model::sec_opt::STD :
        {
            init::subtourEliminationConstrs(model, constrs, x, pInst);
            break;
        }
        case ConfigParameters::model::sec_opt::MTZ :
        {
            init::variablesU(model, u, pInst);
            init::MTZConstrs(model, constrs, u, x, pInst);
            break;
        }
        case ConfigParameters::model::sec_opt::CVRPSEP :
        {
            RAW_LOG_F(INFO, "\tusing lazy and cut (CVRPSEP package)");

            model.set(GRB_IntParam_LazyConstraints, 1); // see [1]
            model.set(GRB_IntParam_PreCrush, 1); // see [2]

            init::depotEdgesEqualsK(model, constrs, x, pInst); // to avoid bug

            pCbSEC = std::make_shared<CallbackSEC>(x, y, pInst);
            model.setCallback(pCbSEC.get());
            break;
        }
        }
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

VrpLp::VrpLp(const std::shared_ptr<const Instance>& pInst,
             const ConfigParameters::model& params) :
    mpInst(pInst),
    mModel(mEnv),
    mpCbSEC(nullptr)
{
    initModel(mModel, m_y, m_u, m_x, mConstrs, mpCbSEC, mpInst, params);
}


bool VrpLp::solve(const ConfigParameters::solver& params)
{
    RAW_LOG_F(INFO, "Solving VRP LP...");
    RAW_LOG_F(INFO, "%s", std::string(80, '-').c_str());
    bool solved = true;

    try
    {
        // set solver parameters
        mModel.set(GRB_IntParam_OutputFlag, params.showLog_);
        mModel.set(GRB_DoubleParam_TimeLimit, params.timeLimit_);
        mModel.set(GRB_IntParam_Threads, params.nbThreads_);
        mModel.set(GRB_StringParam_LogFile, params.logFile_);

        mModel.optimize();

        if (mModel.get(GRB_IntAttr_Status) == GRB_INFEASIBLE ||
            mModel.get(GRB_IntAttr_Status) == GRB_INF_OR_UNBD)
        {
            solved = false;
        }
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

    return solved;
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


void VrpLp::writeModel(std::string path)
{
    DCHECK_F(std::filesystem::is_directory(path), "dir does not exists");
    path += mpInst->getName() + "_irp.lp";

    try
    {
        mModel.write(path);
    }
    catch (GRBException& e)
    {
        RAW_LOG_F(ERROR, "writeModel() exp: %s", e.getMessage().c_str());
    }
    catch (...)
    {
        RAW_LOG_F(ERROR, "writeModel(): Unknown Exception");
    }
}


void VrpLp::writeResultsJSON(std::string path)
{
    DCHECK_F(std::filesystem::is_directory(path), "dir does not exists");
    path += mpInst->getName() + ".json";

    try
    {
        mModel.set(GRB_IntParam_JSONSolDetail, 1);
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