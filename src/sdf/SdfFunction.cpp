#include "SdfFunction.h"

#include "UniformGridSdf.h"
#include "OctreeSdf.h"

bool SdfFunction::saveToFile(const std::string& outputPath)
{
    std::ofstream os(outputPath, std::ios::out | std::ios::binary);
    if(!os.is_open())
    {
        SPDLOG_ERROR("Cannot open file {}", outputPath);
        return false;
    }
    cereal::PortableBinaryOutputArchive archive(os);
    SdfFormat format = getFormat();

    if(format == SdfFormat::GRID)
    {
        archive(format);
        archive(*reinterpret_cast<UniformGridSdf*>(this));
    }
    else if(format == SdfFormat::OCTREE)
    {
        archive(format);
        archive(*reinterpret_cast<OctreeSdf*>(this));
    }
    else
    {
        SPDLOG_ERROR("Unknown format to save");
        return false;
    }

    return true;
}

std::unique_ptr<SdfFunction> SdfFunction::loadFromFile(const std::string& inputPath)
{
    std::ifstream is(inputPath, std::ios::binary);
    if(!is.is_open())
    {
        SPDLOG_ERROR("Cannot open file {}", inputPath);
        return std::unique_ptr<SdfFunction>();
    }
    cereal::PortableBinaryInputArchive archive(is);
    SdfFunction::SdfFormat format = SdfFunction::SdfFormat::NONE;
    archive(format);

    if(format == UniformGridSdf::GRID)
    {
        std::unique_ptr<UniformGridSdf> obj(new UniformGridSdf());
        archive(*obj);
        return obj;
    }
    else if(format == UniformGridSdf::OCTREE)
    {
        std::unique_ptr<OctreeSdf> obj(new OctreeSdf());
        archive(*obj);
        return obj;
    }
    else
    {
        SPDLOG_ERROR("Unknown file format");
        return std::unique_ptr<SdfFunction>();
    }
}