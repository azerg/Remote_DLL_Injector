#include "stdafx.h"
#include "InjectionMngr.h"
#include "StealthInject.h"
#include <Windows.h>
#include <vector>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

std::vector<uint8_t> ReadFileContents(std::string filePath)
{
  std::ifstream testFile(filePath.c_str(), std::ios::binary);
  std::vector<uint8_t> fileContents;
  fileContents.assign(
    std::istreambuf_iterator<char>(testFile),
    std::istreambuf_iterator<char>());
  return fileContents;
}

class InjectionMngr::LogBuff : public std::streambuf
{
public:
  LogBuff(CHListBox& lbLogOutput):
    lbLogOutput_(lbLogOutput)
  {
    setp(0, 0);
  }

  virtual int_type overflow(int_type c = traits_type::eof())
  {
    strMsg_ += c;
    return c;
  }

  virtual int sync() noexcept
  {
    PrintMsgToListBox();
    return 0; // hardcoded success
  }

private:
  void PrintMsgToListBox() noexcept
  {
    auto ext = strMsg_.length();
    if (lbLogOutput_.GetHorizontalExtent() < ext)
    {
      //lbLogOutput_.SetHorizontalExtent(ext);
    }

    lbLogOutput_.AddString(strMsg_.c_str());
    strMsg_.clear();
  }

  std::string strMsg_;
  CHListBox& lbLogOutput_;
};

InjectionMngr::InjectionMngr(CHListBox& lbLogOutput):
  logBuff_{std::make_unique<LogBuff>(lbLogOutput)},
  lbLogOutput_(lbLogOutput)
{}

bool InjectionMngr::DoInject(const char* targetProcessName, const char * dllToInjectPath, InjectionOptions options) const
{
  lbLogOutput_.ResetContent();

  static StealthParamsIn in{};
  static StealthParamsOut out{};
  in.dllToInject = ReadFileContents(dllToInjectPath);
  in.process = targetProcessName;
  // unussed yet. todo(azerg): add to options
  //in.params = {1,2,3,4,5,6,7,8,9,0,9,8,7,6,5,4,3,2,1}; // sample params arr :D
  in.removeExtraSections = options.removeExtraSections;
  in.removePEHeader = options.removePEHeader;
  in.randomHead = options.randomHead;
  in.randomTail = options.randomTail;
  in.randomMax = options.randomMax;
  in.injectWithLocalDll = options.injectWithLocalDll;
  in.localDllPath = (boost::filesystem::current_path() /= "dummyLocal.dll").generic_string();

  StealthInject inj(logBuff_.get());
  SIError err = inj.Inject(&in, &out);

  // delete temp local dll file
  if (in.injectWithLocalDll)
  {
    boost::filesystem::remove(in.localDllPath);
  }

  return err == SI_Success;
}
