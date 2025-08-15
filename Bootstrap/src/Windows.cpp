#ifdef _WIN32
#ifdef USE_WINMAIN_WRAPPER
#include <windows.h>
int main(int argc, char** argv);

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// TODO: Convert command line arguments to argc/argv format
    return main(0, nullptr);
}
#endif
#endif
