////////////////////////////////////////////////////////////////////////////////
/*
 * File: ConfigParameters.cpp
 * Author: Guilherme O. Chagas
 *
 * @brief Configuration input parameters class definition
 * (see ./input/example.cfg).
 *
 * (I'm sorry for my bad english xD)
 *
 * Created on January 9, 2020, 11:58 PM
 * 
 * References:
 */
////////////////////////////////////////////////////////////////////////////////

#include <filesystem>
#include <fstream>
#include <thread>

#include "../include/ext/loguru/loguru.hpp"

#include "../include/config_parameters.hpp"

//////////////////////////////// Helper methods ////////////////////////////////

namespace
{

/**
 * @brief Strings labels of each parameter.
 */
const std::string c_instance_path = "instance_path";
const std::string c_output_dir = "output_dir";
const std::string c_solver_show_log = "solver_show_log";
const std::string c_solver_time_limit = "solver_time_limit";
const std::string c_solver_nb_threads = "solver_nb_threads";
const std::string c_K = "nb_vehicles";
const std::string c_sec_strategy = "sec_strategy";

/**
 * @brief Parse string to boolean.
 * @param: const std::string &: string to be parsed.
 * @return: bool: parsed value.
 */
bool parseBool(const std::string &str)
{
    return str.compare("true") == 0;
}

/**
 * @brief Parse string to unsigned int. It also checks if the input string is
 * set to max. If so, then parse to max value.
 * @param: const std::string &: string to be parsed.
 * @return: std::size_t: parsed value.
*/
std::size_t parseUint(const std::string &str)
{
    if (str == "max")
    {
        return std::thread::hardware_concurrency();
    }

    int val = std::stoi(str);
    CHECK_F(val >= 0, "Input parameter: Invalid value");

    return static_cast<std::size_t>(val);
}

/**
 * @brief.
*/
ConfigParameters::model::sec_opt parseSECOpt(const std::string &str)
{
    if (std::stoi(str) == 0)
    {
        return ConfigParameters::model::sec_opt::CON;
    }
    else if (std::stoi(str) == 1)
    {
        return ConfigParameters::model::sec_opt::STD;
    }
    else if (std::stoi(str) == 2)
    {
        return ConfigParameters::model::sec_opt::MTZ;
    }
    else if (std::stoi(str) == 3)
    {
        return ConfigParameters::model::sec_opt::CVRPSEP;
    }
    else
    {
        RAW_LOG_F(FATAL, "Configuration file: Invalid SEC option");
    }

    return ConfigParameters::model::sec_opt::STD; // to avoid warning
}

/**
 * @brief.
 * @param:.
 * @return:.
 */
std::map<std::string, std::string> readCfgFile(const std::string &config_path)
{
    std::ifstream file(config_path);
    std::string line;

    std::map<std::string, std::string> data;

    while (std::getline(file, line))
    {
        std::size_t begin = line.find_first_not_of( " \f\t\v" );

        if (begin == std::string::npos)
        {
            continue; // skip blank lines
        }

        if (std::string("#").find(line[begin]) != std::string::npos)
        {
            continue; // skip commentary
        }

        std::size_t end = line.find('=', begin);
        std::string key = line.substr(begin, end - begin);

        // (no leading or trailing whitespace allowed)
        key.erase(key.find_last_not_of( " \f\t\v" ) + 1);

        if (key.empty())
        {
            continue; // no blank keys allowed
        }

        // extract the value (no leading or trailing whitespace allowed)
        begin = line.find_first_not_of(" \f\n\r\t\v", end + 1);
        end   = line.find_last_not_of(" \f\n\r\t\v") + 1;

        std::string value = line.substr(begin, end - begin);

        data[key] = value;
    }

    return data;
}

} // anonymous namespace

////////////////////////////////////////////////////////////////////////////////

ConfigParameters::ConfigParameters(const std::string config_path)
{
    CHECK_F(std::filesystem::exists(config_path));
    mData = readCfgFile(config_path);
    setupParameters();
}


std::string ConfigParameters::getInstancePath() const
{
    return mInstancePath;
}


std::string ConfigParameters::getOutputDir() const
{
    return mOutputDir;
}


ConfigParameters::model ConfigParameters::getModelParams() const
{
    return mModelParam;
}


ConfigParameters::solver ConfigParameters::getSolverParams() const
{
    return mSolverParam;
}


void ConfigParameters::setSolverLogFilePath(const std::string& logFile)
{
    mSolverParam.logFile_ = logFile;
}


void ConfigParameters::show() const
{
    RAW_LOG_F(INFO, std::string(80, '-').c_str());
    RAW_LOG_F(INFO, "Parameters");
    for (auto &e : mData)
    {
        std::ostringstream oss;
        oss << std::setw(40) << e.first << std::setw(40) << e.second;
        RAW_LOG_F(INFO, oss.str().c_str());
    }
    RAW_LOG_F(INFO, std::string(80, '-').c_str());
}

/////////////////////////////// private methods ////////////////////////////////

void ConfigParameters::setupParameters()
{
    // ---- General parameters ----
    mInstancePath = mData[c_instance_path];
    mOutputDir = mData[c_output_dir];
    // ---- Solver parameters ----
    mSolverParam.showLog_ = parseBool(mData[c_solver_show_log]);
    mSolverParam.timeLimit_ = parseUint(mData[c_solver_time_limit]);
    mSolverParam.nbThreads_ = parseUint(mData[c_solver_nb_threads]);
    // ---- Model parameters ----
    mModelParam.K_ = std::stoi(mData[c_K]);
    mModelParam.sec_strategy = parseSECOpt(mData[c_sec_strategy]);
}