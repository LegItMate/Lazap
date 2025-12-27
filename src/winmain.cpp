#ifdef _WIN32
#include <windows.h>

// Forward-declare main()
int main();

int WINAPI WinMain(
    HINSTANCE,
    HINSTANCE,
    LPSTR,
    int
) {
    return main();
}
#endif
