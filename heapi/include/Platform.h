#ifndef PLATFORM_H
#define PLATFORM_H

/**
 * Determine which platform is used
 */
#if defined(_WIN32) || defined(_WIN64)
#define PLATFORM_NAME "windows"
#elif defined(__unix__) // *nix, OSX
#define PLATFORM_NAME "unix"
#else
#define PLATFORM_NAME NULL
#endif

#endif
