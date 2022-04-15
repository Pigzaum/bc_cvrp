////////////////////////////////////////////////////////////////////////////////
/*
 * File: helper.cpp
 * Author: Guilherme O. Chagas
 *
 * @brief User helper functions definitions.
 *
 * (I'm sorry for my bad english xD)
 *
 * Created on April 5, 2021, 08:31 AM
 * 
 * References:
 */
////////////////////////////////////////////////////////////////////////////////

#include <sstream>
#include <boost/program_options.hpp>

#include "../../include/ext/loguru/loguru.hpp"

#include "../../include/utils/helper.hpp"
#include "../../include/utils/cmake_config.hpp"

////////////////////////////// Helper functions  ///////////////////////////////

namespace
{

void printVersion()
{
    RAW_LOG_F(INFO, "Branch-and-cut algorithm for the CVRP, version %s",
        PROJECT_VER);
}

} // anonymous namespace

////////////////////////////////////////////////////////////////////////////////

std::string utils::helper::parseCmdLine(int argc, char **argv)
{
    namespace po = boost::program_options;

    try
    {
        po::options_description desc("Usage");
        desc.add_options()
            ("help,h", "Display usage")
            ("version,v", "Display version information")
            ("cfg-file,f", po::value<std::string>(),
                "Configuration file path (see ./input/example.cfg)")
        ;

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help"))
        {
            printVersion();
            std::ostringstream oss;
            oss << "\n" << desc;
            RAW_LOG_F(INFO, "%s", oss.str().c_str());
            std::exit(EXIT_SUCCESS);
        }

        if (vm.count("version"))
        {
            printVersion();
            std::exit(EXIT_SUCCESS);
        }

        if (vm.count("cfg-file"))
        {
            return vm["cfg-file"].as<std::string>();
        }
        else
        {
            RAW_LOG_F(ERROR, "Configuration file path was not given.");
            std::exit(EXIT_FAILURE);
        }
    }
    catch(std::exception& e)
    {
        RAW_LOG_F(ERROR, "Unrecognized flag or missing argument: %s", argv[1]);
        RAW_LOG_F(INFO, "Run 'bc_cvrp --help' for usage");
        std::exit(EXIT_FAILURE);
    }
    catch(...)
    {
        RAW_LOG_F(FATAL, "parseCmdLine - Exception of unknown type!");
    }

    return "";
}