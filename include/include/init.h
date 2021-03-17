#pragma once

#define __INIT__          __attribute__  ((__section__("Code_InitSect")))

#define INIT_CALL_LEVEL(n) __attribute__ ((__section__("Level" #n "_InitSect")))

#define __INIT_CORE__       INIT_CALL_LEVEL(1)
#define __INIT_POSTCORE__   INIT_CALL_LEVEL(2)
#define __INIT_ARCH__       INIT_CALL_LEVEL(3)
#define __INIT_SUBSYSTEM__  INIT_CALL_LEVEL(4)
#define __INIT_FS__         INIT_CALL_LEVEL(5)
#define __INIT_DEV__        INIT_CALL_LEVEL(6)
#define __INIT_LATE__       INIT_CALL_LEVEL(7)
#define __INIT_APP__        INIT_CALL_LEVEL(8)

#if __GNUC__ == 3 && __GNUC_MINOR__ >= 3 || __GNUC__ >= 4
#define __USED__            __attribute__((__used__))
#else
#define __USED__            __attribute__((__used__))
#endif

typedef int (*INIT_FUNC)(void);
typedef struct {
    INIT_FUNC func;
    const char *name;
} INIT_FUNC_PTR;

#define USER_INIT_VERIFY_PTR(sec,name,val) \
    static const __USED__ __attribute__ ((__section__(sec))) __attribute__ ((at(0x080177f0)))  public_key_class name = {val,val}

#define USER_INIT_BOARD_PTR(sec,name,val) \
    static const __USED__ __attribute__ ((__section__(sec))) __attribute__ ((at(0x08017800)))  public_key_class name = {val,val}


#define CORE_INIT(func,name) \
    static const __USED__ __INIT_CORE__      INIT_FUNC_PTR __initcall_##func = {func,name}

#define POSTCORE_INIT(func,name) \
    static const __USED__ __INIT_POSTCORE__  INIT_FUNC_PTR __initcall_##func = {func,name}

#define ARCH_INIT(func,name) \
    static const __USED__ __INIT_ARCH__      INIT_FUNC_PTR __initcall_##func = {func,name}

#define SUBSYSTEM_INIT(func,name) \
    static const __USED__ __INIT_SUBSYSTEM__ INIT_FUNC_PTR __initcall_##func = {func,name}

#define FS_INIT(func,name) \
    static const __USED__ __INIT_FS__        INIT_FUNC_PTR __initcall_##func = {func,name}

#define DEV_INIT(func,name) \
    static const __USED__ __INIT_DEV__       INIT_FUNC_PTR __initcall_##func = {func,name}

#define LATE_INIT(func,name) \
    static const __USED__ __INIT_LATE__      INIT_FUNC_PTR __initcall_##func = {func,name}

#define APP_INIT(func,name) \
    static const __USED__ __INIT_APP__       INIT_FUNC_PTR __initcall_##func = {func,name}

// to remove the followings
#define ARCH_INFO(msg ...)    printf("Init arch: " msg)

#define PLAT_INFO(msg ...)    printf("Init platform: " msg)

#define SUBSYS_INFO(msg ...)  printf("Init subsystem: " msg)

#define DRIVER_INFO(msg ...)  printf("Init driver: " msg)

#define APP_INFO(msg ...)     printf("Init application: " msg)


// fixme
const char* GuGetFuncName(const void *pFuncAddr);

