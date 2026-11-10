#pragma section(".ilya", read)

extern "C" __declspec(allocate(".ilya")) const char ilya_devmark[] =
    "shStudio 10 :: developed by ilya :: ilya.sh.studio";

#pragma comment(linker, "/INCLUDE:_ilya_devmark")
