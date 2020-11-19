////////////////////////////////////////////////////////////////////////////////
/*
 * File: main.cpp
 *
 * @brief Classic Inventory-Routing Problem (IRP) linear program [1].
 * @author Guilherme O. Chagas (guilherme.o.chagas[a]gmail.com)
 * @date This file was created on October 22, 2020, 05:52 PM
 * @warning I'm sorry for my bad English xD.
 * @acknowledgment Special thanks to Ph.D. Leandro C. Coelho and Ph.D. Cleder
 * Marcos Schenekemberg.
 * @copyright GNU General Public License.
 *
 * References:
 * [1] C. Archetti, L. Bertazzi, G. Laporte and M. G. Speranza. A Branch-and-Cut
 * Algorithm for a Vendor-Managed Inventory-Routing Problem. Transportation
 * Science, 41(3), 2007, pp. 382-391.
 */
////////////////////////////////////////////////////////////////////////////////

#include <filesystem>

#include "../include/ext/loguru/loguru.hpp"

#include "../include/config_parameters.hpp"
#include "../include/instance.hpp"
// #include "../include/irp_lp.hpp"


// void buildNsolve(const std::shared_ptr<const Instance>& pInst,
//                  const ConfigParameters& params)
// {
//     pInst->show();

//     Irp_lp irpSolver(pInst, params.getModelParams());
//     irpSolver.solve(params.getSolverParams());

//     irpSolver.writeResultsJSON(params.getOutputDir());
//     irpSolver.writeSolution(params.getOutputDir());
//     irpSolver.writeModel(params.getOutputDir());
//     // irpSolver.writeIis(params.getOutputDir());
// }


int main(int argc, char **argv)
{
    loguru::init(argc, argv);
    CHECK_F(argc == 3, "Invalid number of parameters! Please see usage...");
    CHECK_F(std::string(argv[1]) == "-f", "Unknown flag! Please see usage...");

    ConfigParameters params(argv[2]);
    std::filesystem::create_directory(params.getOutputDir());

    /* Put every log message in "execution.log": */
    {
        using namespace loguru;
        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);
        std::ostringstream oss;
        oss << params.getOutputDir() << "execution_" <<
            std::put_time(&tm, "%d_%m_%Y_%H_%M_%S") << ".log";
        loguru::add_file(oss.str().c_str(), Append, Verbosity_MAX);
    }

    params.show();

    const std::string path = params.getInstancePath();
    if (std::filesystem::is_directory(path))
    {
        /* execute in batch */
    //     for (const auto &f : std::filesystem::directory_iterator(path))
    //     {
    //         RAW_LOG_F(INFO, "executing instance: %s", f.path().c_str());
    //         // auto pInst = std::make_shared<Instance>(f.path());
    //         // buildNsolve(pInst, params);
    //         // RAW_LOG_F(INFO, std::string(80, '=').c_str());
    //     }
    }
    else
    {
        /* single instance execution */
    //     RAW_LOG_F(INFO, "executing instance: %s", path.c_str());
    //     // auto pInst = std::make_shared<Instance>(path);
    //     // buildNsolve(pInst, params);
    }

    return EXIT_SUCCESS;
}